#include "RecoveryGUI.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include "scanner/DeepScanner.h"
#include "scanner/QuickScanner.h"
#include "recovery/RecoveryManager.h"
#include "utils/Logger.h"
#include "utils/DiskIO.h"
#include "utils/ConfigManager.h"
#include "../include/FileQuality.h"
#include "Logger.h"  // Notre nouveau logger

#ifdef _WIN32
#include <windows.h>
#endif

namespace FileRecovery {
namespace GUI {

static GLFWwindow* g_Window = nullptr;

RecoveryGUI::RecoveryGUI() {
    std::memset(progressHistory_, 0, sizeof(progressHistory_));

    // Load configuration from config.ini
    auto& config = Utils::ConfigManager::getInstance();
    if (!config.load("config.ini")) {
        LOG_WARNING("Config file not found, creating default config.ini");
        Utils::ConfigManager::createDefaultConfig("config.ini");
        config.load("config.ini");
    }

    // Apply configuration values
    std::string outputDir = config.getString("Paths", "output_directory", ".\\recovered_files");
    std::string scanFolder = config.getString("Paths", "scan_folder", "");
    threads_ = config.getInt("Performance", "thread_count", 4);
    deepScan_ = config.getBool("Performance", "deep_scan", true);

    // Copy to char arrays for ImGui
    strncpy_s(outputPath_, sizeof(outputPath_), outputDir.c_str(), _TRUNCATE);
    strncpy_s(scanFolderPath_, sizeof(scanFolderPath_), scanFolder.c_str(), _TRUNCATE);

    LOG_INFO("Configuration loaded: threads=" + std::to_string(threads_) +
             ", deepScan=" + std::string(deepScan_ ? "true" : "false"));

    detectAvailableDisks();
    initializeExtensions();

    // Phase 9: Initialize pagination and batch operations
    pagination_ = std::make_unique<ResultsPagination>(100); // 100 items per page
    batchOps_ = std::make_unique<BatchOperations>();

    // Initialize preview and duplicate detection systems
    filePreview_ = std::make_unique<Utils::FilePreview>();
    duplicateDetector_ = std::make_unique<Utils::DuplicateDetector>();
}

RecoveryGUI::~RecoveryGUI() {
    stopRecovery();
    if (recoveryThread_ && recoveryThread_->joinable()) {
        recoveryThread_->join();
    }
    shutdown();
}

bool RecoveryGUI::initialize() {
    LOG_INFO("RecoveryGUI::initialize() called");
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        LOG_ERROR("Failed to initialize GLFW");
        return false;
    }
    LOG_INFO("GLFW initialized successfully");

    // GL 3.3 + GLSL 330
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    g_Window = glfwCreateWindow(1400, 900, "File Recovery Tool - Modern Edition", nullptr, nullptr);
    if (!g_Window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(g_Window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Apply modern theme
    applyModernTheme();

    return true;
}

void RecoveryGUI::applyModernTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Modern dark theme with purple/blue accents
    colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.09f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.10f, 0.10f, 0.13f, 0.95f);
    colors[ImGuiCol_Border]                 = ImVec4(0.20f, 0.20f, 0.25f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.20f, 0.20f, 0.26f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.24f, 0.24f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.10f, 0.10f, 0.13f, 0.75f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.28f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.35f, 0.35f, 0.42f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.42f, 0.42f, 0.50f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.60f, 0.40f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.55f, 0.35f, 0.85f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.65f, 0.45f, 0.95f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.50f, 0.30f, 0.80f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.60f, 0.40f, 0.90f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.70f, 0.50f, 1.00f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.45f, 0.25f, 0.75f, 0.80f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.55f, 0.35f, 0.85f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.65f, 0.45f, 0.95f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.50f, 0.30f, 0.80f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.60f, 0.40f, 0.90f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.70f, 0.50f, 1.00f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.35f, 0.20f, 0.60f, 0.86f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.55f, 0.35f, 0.85f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.50f, 0.30f, 0.80f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.25f, 0.15f, 0.45f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.35f, 0.20f, 0.60f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.60f, 0.40f, 0.90f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.70f, 0.50f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.55f, 0.35f, 0.85f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.65f, 0.45f, 0.95f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.50f, 0.30f, 0.80f, 0.35f);

    // Rounded corners
    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 6.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding      = 6.0f;
    style.TabRounding       = 6.0f;

    // Padding
    style.WindowPadding     = ImVec2(12, 12);
    style.FramePadding      = ImVec2(10, 6);
    style.ItemSpacing       = ImVec2(10, 8);
    style.ItemInnerSpacing  = ImVec2(8, 6);
}

void RecoveryGUI::run() {
    while (!glfwWindowShouldClose(g_Window)) {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render our UI
        renderMainWindow();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(g_Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.09f, 0.09f, 0.12f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(g_Window);
    }
}

void RecoveryGUI::renderMainWindow() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("MainWindow", nullptr, window_flags);

    // Header
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "FILE RECOVERY TOOL");
    ImGui::PopFont();
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 200);
    ImGui::TextDisabled("Modern Edition v1.0");
    
    ImGui::Separator();
    ImGui::Spacing();

    // Main content area - split into panels
    float leftPanelWidth = 400.0f;
    
    // Left panel - Configuration
    ImGui::BeginChild("LeftPanel", ImVec2(leftPanelWidth, -1), true);
    renderConfigPanel();
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel - Progress and Results
    ImGui::BeginChild("RightPanel", ImVec2(0, -1), true);
    
    // Progress section (top)
    ImGui::BeginChild("ProgressSection", ImVec2(0, 200), true);
    renderProgressPanel();
    ImGui::EndChild();
    
    ImGui::Spacing();
    
    // Results section (bottom)
    ImGui::BeginChild("ResultsSection", ImVec2(0, -1), true);
    renderResultsPanel();
    ImGui::EndChild();
    
    ImGui::EndChild();

    ImGui::End();

    // Render popup panels
    if (showPreview_) {
        renderPreviewPanel();
    }
    if (showDuplicates_) {
        renderDuplicatePanel();
    }

    // About dialog
    if (showAbout_) {
        ImGui::OpenPopup("About");
        if (ImGui::BeginPopupModal("About", &showAbout_, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("File Recovery Tool - Modern Edition");
            ImGui::Separator();
            ImGui::Text("Version: 1.0.0");
            ImGui::Text("Created with Dear ImGui");
            ImGui::Spacing();
            ImGui::Text("Advanced file recovery with quality analysis");
            ImGui::Spacing();
            if (ImGui::Button("Close", ImVec2(120, 0))) {
                showAbout_ = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void RecoveryGUI::renderConfigPanel() {
    ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "CONFIGURATION");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Disk selection - DROPDOWN
    ImGui::Text("Source Disk");
    ImGui::SetNextItemWidth(-1);
    if (!availableDisks_.empty()) {
        if (ImGui::BeginCombo("##disk", availableDisks_[selectedDiskIndex_].c_str())) {
            for (size_t i = 0; i < availableDisks_.size(); i++) {
                bool isSelected = (selectedDiskIndex_ == static_cast<int>(i));
                if (ImGui::Selectable(availableDisks_[i].c_str(), isSelected)) {
                    selectedDiskIndex_ = static_cast<int>(i);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    } else {
        ImGui::TextDisabled("No disks detected");
    }
    ImGui::Spacing();

    // Output path
    ImGui::Text("Output Directory");
    ImGui::SetNextItemWidth(-70);
    ImGui::InputText("##output", outputPath_, sizeof(outputPath_));
    ImGui::SameLine();
    if (ImGui::Button("Browse", ImVec2(60, 0))) {
        // Open folder picker using Windows Shell
        std::string command = "powershell -Command \"Add-Type -AssemblyName System.Windows.Forms; $f = New-Object System.Windows.Forms.FolderBrowserDialog; $f.Description = 'Select Output Folder'; $f.SelectedPath = '" + std::string(outputPath_) + "'; if($f.ShowDialog() -eq 'OK'){ Write-Output $f.SelectedPath }\" > temp_folder.txt";
        system(command.c_str());
        
        // Read selected folder
        std::ifstream tempFile("temp_folder.txt");
        if (tempFile.good()) {
            std::string selectedPath;
            std::getline(tempFile, selectedPath);
            tempFile.close();
            std::filesystem::remove("temp_folder.txt");
            
            if (!selectedPath.empty() && selectedPath.length() < sizeof(outputPath_)) {
                strcpy_s(outputPath_, selectedPath.c_str());
            }
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Browse for output folder");
    }
    ImGui::Spacing();

    // File extensions - CHECKBOXES
    ImGui::Text("File Extensions to Recover");
    ImGui::BeginChild("ExtensionsScroll", ImVec2(0, 150), true);
    for (auto& ext : extensions_) {
        ImGui::Checkbox(ext.name.c_str(), &ext.selected);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", ext.description.c_str());
        }
    }
    ImGui::EndChild();
    ImGui::Spacing();

    // Filter (optional)
    ImGui::Text("Filter (Optional)");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##filter", filter_, sizeof(filter_));
    ImGui::Spacing();

    ImGui::Separator();
    ImGui::Spacing();

    // Scan options
    ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "SCAN OPTIONS");
    ImGui::Spacing();

    ImGui::Checkbox("Deep Scan", &deepScan_);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Scan entire disk for deleted files (slower but more thorough)");
    }

    // Disable "Scan Specific Folder" when Deep Scan is enabled (Deep Scan has no metadata)
    bool canFilterByFolder = !deepScan_;
    if (!canFilterByFolder) {
        ImGui::BeginDisabled();
        scanSpecificFolder_ = false; // Auto-uncheck when Deep Scan is enabled
    }
    
    ImGui::Checkbox("Scan Specific Folder Only", &scanSpecificFolder_);
    if (ImGui::IsItemHovered()) {
        if (deepScan_) {
            ImGui::SetTooltip("❌ NOT AVAILABLE with Deep Scan\n"
                             "Deep Scan reads raw disk sectors without file system metadata.\n"
                             "Use Quick Scan (uncheck Deep Scan) to filter by folder.");
        } else {
            ImGui::SetTooltip("Filter recovered files by their original folder path\n"
                             "Uses Quick Scan to read file system metadata (MFT/FAT).\n"
                             "Works for recently deleted files.");
        }
    }
    
    if (!canFilterByFolder) {
        ImGui::EndDisabled();
    }
    
    if (scanSpecificFolder_) {
        ImGui::Indent();
        ImGui::Text("Folder Path:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##scanfolder", scanFolderPath_, sizeof(scanFolderPath_));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Example: C:\\Users\\YourName\\AppData\\Local\\Octopath_Traveler2\\Saved\\SaveGames");
        }
        ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "⚠ Scans entire disk, filters by folder path");
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Only keeps files matching the target folder");
        ImGui::Unindent();
    }

    ImGui::Text("Threads:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::SliderInt("##threads", &threads_, 1, 16);

    ImGui::Spacing();
    
    // Phase 8: Optimization options
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.7f, 0.9f, 0.3f, 1.0f), "PERFORMANCE OPTIONS");
    ImGui::Spacing();
    
    ImGui::Checkbox("Smart Scan (Skip Empty Sectors)", &enableSmartScan_);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Skip sectors filled with zeros - 50-80%% faster! Recommended.");
    }
    
    ImGui::Checkbox("Large Buffers (1MB blocks)", &useLargeBuffers_);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Use larger read buffers for faster I/O. Uses more RAM.");
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Action buttons
    float buttonWidth = (ImGui::GetContentRegionAvail().x - 10) / 2;
    
    if (!isRecovering_) {
        if (ImGui::Button("START RECOVERY", ImVec2(buttonWidth * 2 + 10, 50))) {
            startRecovery();
        }
    } else {
        if (ImGui::Button("STOP", ImVec2(buttonWidth, 50))) {
            stopRecovery();
        }
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::Button("RUNNING...", ImVec2(buttonWidth, 50));
        ImGui::EndDisabled();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // Info section
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f, 0.7f, 1.0f, 1.0f), "INFORMATION");
    ImGui::Spacing();
    ImGui::TextWrapped("This tool recovers deleted files from your disk. Deep scan mode analyzes the entire disk for file signatures.");
    ImGui::Spacing();
    
    if (ImGui::Button("About", ImVec2(-1, 0))) {
        showAbout_ = true;
    }
}

void RecoveryGUI::renderProgressPanel() {
    ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "PROGRESS");
    ImGui::Spacing();

    if (isRecovering_) {
        // Progress bar
        float currentProgress = progress_.load();
        char progressText[64];
        snprintf(progressText, sizeof(progressText), "%.1f%%", currentProgress * 100.0f);
        ImGui::ProgressBar(currentProgress, ImVec2(-1, 30), progressText);
        
        ImGui::Spacing();
        
        // Stats
        ImGui::Columns(4, "stats", false);
        
        ImGui::Text("Files Found");
        ImGui::TextColored(ImVec4(0.6f, 0.9f, 0.6f, 1.0f), "%d", filesFound_.load());
        
        ImGui::NextColumn();
        ImGui::Text("Bytes Scanned");
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "%.2f GB", bytesScanned_.load() / (1024.0 * 1024.0 * 1024.0));
        
        ImGui::NextColumn();
        ImGui::Text("Scan Speed");
        float speed = scanSpeedMBps_.load();
        if (speed > 0) {
            ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "%.1f MB/s", speed);
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Calculating...");
        }
        
        ImGui::NextColumn();
        ImGui::Text("Est. Time");
        // Phase 8: Simple ETA calculation based on progress
        if (speed > 5.0f && currentProgress > 0.05f) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - scanStartTime_).count();
            int secondsTotal = static_cast<int>(elapsed / currentProgress);
            int secondsRemaining = secondsTotal - static_cast<int>(elapsed);
            if (secondsRemaining > 0) {
                int minutes = secondsRemaining / 60;
                int seconds = secondsRemaining % 60;
                ImGui::TextColored(ImVec4(0.6f, 0.9f, 0.9f, 1.0f), "%d:%02d", minutes, seconds);
            } else {
                ImGui::TextColored(ImVec4(0.6f, 0.9f, 0.9f, 1.0f), "Almost done...");
            }
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Calculating...");
        }
        
        ImGui::Columns(1);
        ImGui::Spacing();
        
        // Phase 8: Show skipped sectors if Smart Scan enabled
        if (enableSmartScan_ && sectorsSkipped_ > 0) {
            ImGui::Separator();
            ImGui::Text("Sectors Skipped (empty): ");
            ImGui::SameLine();
            uint64_t skipped = sectorsSkipped_;
            ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1.0f), "%llu", skipped);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Smart Scan skipped these empty sectors");
            }
        }
        
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.5f, 1.0f), "%.2f GB", totalBytes_ / (1024.0 * 1024.0 * 1024.0));
        
        ImGui::Columns(1);
        
        ImGui::Spacing();
        
        // Progress graph
        progressHistory_[progressHistoryIndex_] = currentProgress;
        progressHistoryIndex_ = (progressHistoryIndex_ + 1) % 100;
        
        ImGui::PlotLines("##progress", progressHistory_, 100, 0, nullptr, 0.0f, 1.0f, ImVec2(-1, 60));
        
    } else {
        ImGui::TextDisabled("Ready to start recovery");
        ImGui::Spacing();
        ImGui::TextWrapped("Configure your recovery options in the left panel and click START RECOVERY to begin.");
    }
}

void RecoveryGUI::renderResultsPanel() {
    ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "RECOVERED FILES");
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 100);
    ImGui::TextDisabled("(%zu files)", recoveredFiles_.size());
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Toolbar with Preview and Duplicate Detection buttons
    if (!recoveredFiles_.empty()) {
        if (ImGui::Button("🔍 Preview Selected")) {
            if (selectedFileIndex_ >= 0 && selectedFileIndex_ < static_cast<int>(recoveredFiles_.size())) {
                const auto& selectedFile = recoveredFiles_[selectedFileIndex_];
                if (std::filesystem::exists(selectedFile.path)) {
                    // Extract file type from filename
                    std::string fileType;
                    size_t dotPos = selectedFile.filename.rfind('.');
                    if (dotPos != std::string::npos) {
                        fileType = selectedFile.filename.substr(dotPos + 1);
                    }
                    currentPreview_ = filePreview_->generatePreview(selectedFile.path, fileType);
                    showPreview_ = true;
                }
            }
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Preview the selected file (image, text, or hex)");
        }

        ImGui::SameLine();
        if (ImGui::Button("🔎 Find Duplicates")) {
            if (!duplicatesAnalyzed_) {
                // Analyze files for duplicates
                duplicateDetector_->clearFiles();
                for (const auto& file : recoveredFiles_) {
                    if (std::filesystem::exists(file.path)) {
                        duplicateDetector_->addFile(file.path);
                    }
                }
                duplicateGroups_ = duplicateDetector_->findDuplicates();
                duplicatesAnalyzed_ = true;
            }
            showDuplicates_ = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Find and manage duplicate files");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }

    // Filters section
    if (!recoveredFiles_.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "FILTERS");
        
        // Search filter
        ImGui::Text("Search:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputText("##search", searchFilter_, sizeof(searchFilter_));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Filter by filename");
        }
        
        ImGui::SameLine();
        ImGui::Text("Quality:");
        
        // Quality filters
        ImGui::SameLine();
        ImGui::Checkbox("Excellent", &filterByQuality_[0]);
        ImGui::SameLine();
        ImGui::Checkbox("Good", &filterByQuality_[1]);
        ImGui::SameLine();
        ImGui::Checkbox("Poor", &filterByQuality_[2]);
        ImGui::SameLine();
        ImGui::Checkbox("Corrupted", &filterByQuality_[3]);
        ImGui::SameLine();
        ImGui::Checkbox("Dead", &filterByQuality_[4]);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }

    if (recoveredFiles_.empty()) {
        ImGui::TextDisabled("No files recovered yet");
        return;
    }

    // Filter files based on search and quality filters
    std::vector<size_t> filteredIndices;
    std::string searchLower = searchFilter_;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), 
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    
    for (size_t i = 0; i < recoveredFiles_.size(); i++) {
        const auto& file = recoveredFiles_[i];
        
        // Check quality filter
        int qualityIdx = static_cast<int>(file.quality);
        if (!filterByQuality_[qualityIdx]) continue;
        
        // Check search filter
        if (searchLower.length() > 0) {
            std::string filenameLower = file.filename;
            std::transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), 
                          [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (filenameLower.find(searchLower) == std::string::npos) continue;
        }
        
        filteredIndices.push_back(i);
    }

    // Show filtered count
    if (filteredIndices.size() != recoveredFiles_.size()) {
        ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), 
            "Showing %zu of %zu files", filteredIndices.size(), recoveredFiles_.size());
        ImGui::Spacing();
    }
    
    // Phase 9: Batch Operations Controls
    if (!filteredIndices.empty()) {
        batchOps_->renderBatchControls(filteredIndices.size());
        ImGui::Spacing();
    }
    
    // Phase 9: Update pagination with filtered count
    pagination_->setTotalItems(filteredIndices.size());
    
    // Get pagination range
    size_t startIdx = pagination_->getStartIndex();
    size_t endIdx = pagination_->getEndIndex();

    // Table of recovered files (only show current page)
    if (ImGui::BeginTable("FilesTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                          ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable)) {
        ImGui::TableSetupColumn("☑", ImGuiTableColumnFlags_WidthFixed, 30); // Checkbox column
        ImGui::TableSetupColumn("Filename", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableSetupColumn("Quality", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableSetupColumn("Zeros", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableHeadersRow();

        // Handle sorting
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs()) {
            if (sortSpecs->SpecsDirty) {
                if (sortSpecs->SpecsCount > 0) {
                    sortColumn_ = sortSpecs->Specs[0].ColumnIndex;
                    sortAscending_ = sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending;
                    
                    // Sort filtered indices
                    std::sort(filteredIndices.begin(), filteredIndices.end(), 
                        [this](size_t a, size_t b) {
                            const auto& fileA = recoveredFiles_[a];
                            const auto& fileB = recoveredFiles_[b];
                            bool result = false;
                            
                            switch (sortColumn_) {
                                case 1: // Filename (column 0 is checkbox now)
                                    result = fileA.filename < fileB.filename;
                                    break;
                                case 2: // Size
                                    result = fileA.size < fileB.size;
                                    break;
                                case 3: // Quality
                                    result = static_cast<int>(fileA.quality) < static_cast<int>(fileB.quality);
                                    break;
                                case 4: // Zeros
                                    result = fileA.zeroPercentage < fileB.zeroPercentage;
                                    break;
                            }
                            
                            return sortAscending_ ? result : !result;
                        });
                }
                sortSpecs->SpecsDirty = false;
            }
        }

        // Render only files in current page
        for (size_t i = startIdx; i < endIdx && i < filteredIndices.size(); i++) {
            size_t idx = filteredIndices[i];
            const auto& file = recoveredFiles_[idx];
            
            ImGui::TableNextRow();
            
            // Checkbox column
            ImGui::TableNextColumn();
            bool isChecked = batchOps_->isSelected(idx);
            if (ImGui::Checkbox(("##chk" + std::to_string(idx)).c_str(), &isChecked)) {
                batchOps_->toggleSelection(idx);
            }
            
            // Filename column
            ImGui::TableNextColumn();
            bool isSelected = (selectedFileIndex_ == static_cast<int>(idx));
            if (ImGui::Selectable(file.filename.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                selectedFileIndex_ = static_cast<int>(idx);
            }
            
            // Double-click to open file
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                std::string command = "start \"\" \"" + file.path + "\"";
                system(command.c_str());
                std::cout << "📂 Opening file: " << file.path << std::endl;
            }
            
            // Right-click context menu
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Open File")) {
                    std::string command = "start \"\" \"" + file.path + "\"";
                    system(command.c_str());
                }
                if (ImGui::MenuItem("Open in Explorer")) {
                    std::string command = "explorer /select,\"" + file.path + "\"";
                    system(command.c_str());
                }
                if (ImGui::MenuItem("Copy Path")) {
                    ImGui::SetClipboardText(file.path.c_str());
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Delete File", nullptr, false, std::filesystem::exists(file.path))) {
                    try {
                        std::filesystem::remove(file.path);
                        std::cout << "🗑️ Deleted file: " << file.path << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "❌ Failed to delete file: " << e.what() << std::endl;
                    }
                }
                ImGui::EndPopup();
            }
            
            // Size column
            ImGui::TableNextColumn();
            ImGui::Text("%.2f MB", file.size / (1024.0 * 1024.0));
            
            // Quality column
            ImGui::TableNextColumn();
            // Color-coded quality
            ImVec4 qualityColor;
            const char* qualityText;
            switch (file.quality) {
                case FileQualityLevel::EXCELLENT:
                    qualityColor = ImVec4(0.3f, 0.9f, 0.3f, 1.0f);
                    qualityText = "✓ Excellent";
                    break;
                case FileQualityLevel::GOOD:
                    qualityColor = ImVec4(0.6f, 0.9f, 0.3f, 1.0f);
                    qualityText = "✓ Good";
                    break;
                case FileQualityLevel::POOR:
                    qualityColor = ImVec4(0.9f, 0.7f, 0.3f, 1.0f);
                    qualityText = "⚠ Poor";
                    break;
                case FileQualityLevel::CORRUPTED:
                    qualityColor = ImVec4(0.9f, 0.4f, 0.2f, 1.0f);
                    qualityText = "⚠ Corrupted";
                    break;
                case FileQualityLevel::DEAD:
                    qualityColor = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);
                    qualityText = "☠ Dead";
                    break;
                default:
                    qualityColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
                    qualityText = "Unknown";
            }
            ImGui::TextColored(qualityColor, "%s", qualityText);
            
            // Zeros column
            ImGui::TableNextColumn();
            ImGui::Text("%.1f%%", file.zeroPercentage);
        }

        ImGui::EndTable();
    }
    
    // Phase 9: Render pagination controls
    if (!filteredIndices.empty()) {
        pagination_->render();
    }
    
    // File details section
    if (selectedFileIndex_ >= 0 && selectedFileIndex_ < static_cast<int>(recoveredFiles_.size())) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        renderFileDetails();
    }
}

void RecoveryGUI::renderFileDetails() {
    const auto& file = recoveredFiles_[selectedFileIndex_];
    
    ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "FILE DETAILS");
    ImGui::Spacing();
    
    ImGui::Text("Path:"); ImGui::SameLine();
    ImGui::TextWrapped("%s", file.path.c_str());
    
    ImGui::Text("Size:"); ImGui::SameLine();
    ImGui::Text("%.2f MB (%zu bytes)", file.size / (1024.0 * 1024.0), file.size);
    
    ImGui::Text("Zero Percentage:"); ImGui::SameLine();
    ImGui::Text("%.2f%%", file.zeroPercentage);
    
    ImGui::Spacing();
    
    if (ImGui::Button("Open in Explorer", ImVec2(150, 0))) {
        std::string command = "explorer /select,\"" + file.path + "\"";
        system(command.c_str());
    }
}

void RecoveryGUI::startRecovery() {
    if (isRecovering_) return;
    
    isRecovering_ = true;
    stopRequested_ = false;
    progress_ = 0.0f;
    filesFound_ = 0;
    bytesScanned_ = 0;
    scanSpeedMBps_ = 0.0f;
    recoveredFiles_.clear();
    selectedFileIndex_ = -1;
    scanStartTime_ = std::chrono::steady_clock::now();
    
    // Get selected extensions
    std::vector<std::string> selectedExts;
    for (const auto& ext : extensions_) {
        if (ext.selected) {
            selectedExts.push_back(ext.name);
        }
    }
    
    if (selectedExts.empty()) {
        std::cerr << "❌ No extensions selected!" << std::endl;
        isRecovering_ = false;
        return;
    }
    
    // Start recovery in background thread
    if (recoveryThread_ && recoveryThread_->joinable()) {
        recoveryThread_->join();
    }
    
    recoveryThread_ = std::make_unique<std::thread>([this, selectedExts]() {
        try {
            LOG_DEBUG("Recovery thread started");
            
            std::string diskPath = availableDisks_[selectedDiskIndex_];
            LOG_DEBUG("Selected disk: " + diskPath);

            // Extract just the drive letter (e.g., "C:" from "C: - Local Disk (Windows)")
            if (diskPath.length() >= 2 && diskPath[1] == ':') {
                diskPath = diskPath.substr(0, 2);
            }
            LOG_DEBUG("Cleaned disk path: " + diskPath);

            std::string outputPath = outputPath_;
            LOG_DEBUG("Output path: " + outputPath);
            
            std::cout << "\n🚀 Starting recovery..." << std::endl;
            std::cout << "📀 Disk: " << diskPath << std::endl;
            std::cout << "📁 Output: " << outputPath << std::endl;
            std::cout << "📋 Extensions: " << selectedExts.size() << " types" << std::endl;
            
            LOG_INFO("=== Starting Recovery ===");
            LOG_INFO("Disk: " + diskPath);
            LOG_INFO("Output: " + outputPath);
            LOG_INFO("Extensions: " + std::to_string(selectedExts.size()) + " types");
            
            if (scanSpecificFolder_) {
                std::cout << "📂 Folder filter mode: " << scanFolderPath_ << std::endl;
                std::cout << "   (Scans entire disk, keeps only files from target folder)" << std::endl;
                LOG_INFO("Folder filter mode: " + std::string(scanFolderPath_));
            } else {
                std::cout << "🌐 Full disk scan mode (no folder filter)" << std::endl;
                LOG_INFO("Full disk scan mode");
            }
            
            // Create output directory
            LOG_DEBUG("Creating output directory...");
            std::filesystem::create_directories(outputPath);
            LOG_DEBUG("Output directory created");
            
            // Initialize scanner based on scan mode
            if (deepScan_) {
                LOG_DEBUG("Creating DeepScanner...");
                auto deepScanner = std::make_unique<DeepScanner>();
                
                // Phase 8: Set optimization options (only for DeepScanner)
                deepScanner->setSmartScanEnabled(enableSmartScan_);
                deepScanner->setLargeBuffersEnabled(useLargeBuffers_);
                sectorsSkipped_ = 0; // Reset counter
                
                scanner_ = std::move(deepScanner);
                LOG_DEBUG("DeepScanner created");
                
                if (enableSmartScan_) {
                    std::cout << "   ⚡ Smart Scan enabled (will skip empty sectors)" << std::endl;
                }
                if (useLargeBuffers_) {
                    std::cout << "   📦 Large buffers enabled (1MB blocks)" << std::endl;
                }
            } else {
                LOG_DEBUG("Creating QuickScanner (MFT/FAT scan)...");
                scanner_ = std::make_unique<QuickScanner>();
                LOG_DEBUG("QuickScanner created");
                std::cout << "   🚀 Quick Scan mode (reads file system metadata)" << std::endl;
            }
            
            LOG_DEBUG("Creating RecoveryManager...");
            recoveryManager_ = std::make_unique<RecoveryManager>();
            LOG_DEBUG("RecoveryManager created");
            
            // Setup callbacks
            LOG_DEBUG("Setting up progress callback...");
            scanner_->setProgressCallback([this](float prog, const std::string& status) {
                try {
                    progress_.store(prog);
                    
                    // Phase 8: Update skipped sectors counter (only for DeepScanner)
                    if (deepScan_ && scanner_) {
                        auto* deepScanner = dynamic_cast<DeepScanner*>(scanner_.get());
                        if (deepScanner) {
                            sectorsSkipped_.store(deepScanner->getSectorsSkipped());
                        }
                    }
                    
                    // Calculate scan speed
                    auto now = std::chrono::steady_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - scanStartTime_).count();
                    if (elapsed > 0) {
                        size_t bytesScanned = bytesScanned_.load();
                        float speedMBps = (bytesScanned / (1024.0f * 1024.0f)) / elapsed;
                        scanSpeedMBps_.store(speedMBps);
                    }
                    
                    std::cout << "📊 Progress: " << (int)(prog * 100) << "% - " << status << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] Exception in progress callback: " << e.what() << std::endl;
                } catch (...) {
                    std::cerr << "[ERROR] Unknown exception in progress callback!" << std::endl;
                }
            });
            LOG_DEBUG("Progress callback set");
            
            LOG_DEBUG("Setting up file found callback...");
            
            // Create a DiskIO for file recovery in callbacks
            LOG_DEBUG("Creating DiskIO for recovery...");
            auto diskIO = std::make_shared<DiskIO>();
            if (!diskIO->openDevice(diskPath)) {
                std::cerr << "[ERROR] Failed to open device for file recovery callbacks!" << std::endl;
            }
            LOG_DEBUG("DiskIO created for callbacks");
            
            scanner_->setFileFoundCallback([this, outputPath, diskIO, scanFolder = std::string(scanFolderPath_), isSpecificFolder = scanSpecificFolder_](const RecoveredFile& file) {
                try {
                    // Note: Deep Scan doesn't have file.originalName metadata, so folder filtering won't work
                    // The UI now disables "Scan Specific Folder" when Deep Scan is enabled
                    
                    if (isSpecificFolder && !scanFolder.empty()) {
                        // This code path should not execute with Deep Scan, but keep it for Quick Scan
                        if (file.originalName.empty()) {
                            // No metadata available (happens with Deep Scan)
                            return;
                        }
                        
                        // Check if the original name contains the specific folder path
                        std::string originalLower = file.originalName;
                        std::string scanFolderLower = scanFolder;
                        std::transform(originalLower.begin(), originalLower.end(), originalLower.begin(), ::tolower);
                        std::transform(scanFolderLower.begin(), scanFolderLower.end(), scanFolderLower.begin(), ::tolower);
                        
                        if (originalLower.find(scanFolderLower) == std::string::npos) {
                            // File is not in the specific folder, skip it
                            return;
                        }
                    }
                    
                    // No arbitrary file limit - rely on system memory management
                    // Log every 1000 files to track progress
                    size_t currentCount = filesFound_.fetch_add(1);
                    if (currentCount % 1000 == 0 && currentCount > 0) {
                        LOG_INFO("Found " + std::to_string(currentCount) + " files so far...");
                    }
                    
                    std::cout << "✓ Found file: " << file.originalName
                             << " (size: " << file.fileSize << " bytes)" << std::endl;
                    
                    // Generate filename
                    std::string filename = file.originalName.empty() ? 
                        ("recovered_" + std::to_string(file.diskOffset) + "." + file.detectedType) :
                        file.originalName;
                    std::string filepath = outputPath + "\\" + filename;
                    
                    // Actually recover the file using RecoveryManager!
                    std::cout << "💾 Recovering file: " << filename << std::endl;
                    
                    bool success = false;
                    FileQualityLevel qualityLevel = FileQualityLevel::GOOD; // Default to GOOD
                    float zeroPercentage = 0.0f;
                    
                    if (recoveryManager_ && diskIO) {
                        success = recoveryManager_->recoverFile(file, filepath, *diskIO);
                        
                        if (success) {
                            // Analyze quality of the recovered file
                            FileQuality qualityAnalyzer;
                            std::ifstream inFile(filepath, std::ios::binary);
                            if (inFile) {
                                // Read file into buffer
                                inFile.seekg(0, std::ios::end);
                                size_t fileSize = inFile.tellg();
                                inFile.seekg(0, std::ios::beg);
                                
                                ByteArray fileData(fileSize);
                                inFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);
                                inFile.close();
                                
                                // Analyze
                                FileQualityReport qualityReport = qualityAnalyzer.analyzeBuffer(fileData, file.detectedType);
                                qualityLevel = qualityReport.level;
                                zeroPercentage = qualityReport.zeroPercentage;
                                
                                std::cout << "   Quality: " << FileQuality::getQualityDescription(qualityLevel) 
                                         << " (" << static_cast<int>(zeroPercentage) << "% zeros)" << std::endl;
                            }
                        }
                    }
                    
                    if (!success) {
                        std::cerr << "[ERROR] Failed to recover file: " << filename << std::endl;
                    }
                    
                    // Add to results (thread-safe)
                    RecoveredFileInfo info;
                    info.filename = filename;
                    info.size = file.fileSize;
                    info.quality = qualityLevel;
                    info.path = filepath;
                    info.zeroPercentage = zeroPercentage;
                    
                    LOG_DEBUG("About to lock mutex...");
                    std::lock_guard<std::mutex> lock(dataMutex_);
                    LOG_DEBUG("Mutex locked, adding to vector...");
                    recoveredFiles_.push_back(info);
                    LOG_DEBUG("File added successfully. Total files: " + std::to_string(recoveredFiles_.size()));
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] Exception in file found callback: " << e.what() << std::endl;
                    LOG_ERROR(std::string("Exception in file found callback: ") + e.what());
                } catch (...) {
                    std::cerr << "[ERROR] Unknown exception in file found callback!" << std::endl;
                    LOG_ERROR("Unknown exception in file found callback");
                }
            });
            LOG_DEBUG("File found callback set");
            
            // Configure scan for each extension
            LOG_DEBUG("Starting scan loop for " << selectedExts.size() << " extensions...");
            for (const auto& ext : selectedExts) {
                if (stopRequested_) {
                    std::cout << "\n⏹️ Scan stopped by user" << std::endl;
                    break;
                }
                
                std::cout << "\n🔍 Scanning for ." << ext << " files..." << std::endl;
                
                // Setup disk info - Get actual disk size
                LOG_DEBUG("Setting up DiskInfo...");
                DiskInfo diskInfo;
                diskInfo.devicePath = diskPath;
                diskInfo.sectorSize = 512;
                
                // Get disk size and filesystem type using Windows API
                std::string rootPath = diskPath + "\\";
                ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
                if (GetDiskFreeSpaceExA(rootPath.c_str(), &freeBytesAvailable, 
                                       &totalBytes, &totalFreeBytes)) {
                    diskInfo.totalSize = totalBytes.QuadPart;
                    diskInfo.freeSize = totalFreeBytes.QuadPart;
                    diskInfo.usedSize = diskInfo.totalSize - diskInfo.freeSize;
                    
                    LOG_DEBUG("Disk size: " << (diskInfo.totalSize / (1024*1024*1024)) << " GB");
                    
                    // Detect filesystem type
                    char fileSystemName[256];
                    DWORD serialNumber, maxComponentLen, fileSystemFlags;
                    if (GetVolumeInformationA(rootPath.c_str(), nullptr, 0, &serialNumber, 
                                              &maxComponentLen, &fileSystemFlags, 
                                              fileSystemName, sizeof(fileSystemName))) {
                        std::string fsName(fileSystemName);
                        LOG_DEBUG("Filesystem: " + fsName);

                        if (fsName == "NTFS") {
                            diskInfo.fsType = FilesystemType::NTFS;
                        } else if (fsName == "FAT32" || fsName == "FAT") {
                            diskInfo.fsType = FilesystemType::FAT32;
                        } else if (fsName == "exFAT") {
                            diskInfo.fsType = FilesystemType::exFAT;
                        } else {
                            diskInfo.fsType = FilesystemType::UNKNOWN;
                            std::cout << "[WARNING] Unknown filesystem: " << fsName << std::endl;
                        }
                    } else {
                        std::cout << "[WARNING] Could not detect filesystem type, assuming NTFS" << std::endl;
                        diskInfo.fsType = FilesystemType::NTFS; // Default to NTFS on Windows
                    }
                    
                    if (scanSpecificFolder_) {
                        std::cout << "[INFO] Scanning entire disk with folder filter: " << scanFolderPath_ << std::endl;
                        std::cout << "       Will only keep files matching the folder path" << std::endl;
                    }
                } else {
                    std::cerr << "[ERROR] Failed to get disk size!" << std::endl;
                    diskInfo.totalSize = 500ULL * 1024 * 1024 * 1024; // Default 500GB
                    LOG_DEBUG("Using default size: 500 GB");
                }
                LOG_DEBUG("DiskInfo configured");
                
                // Setup scan config
                LOG_DEBUG("Setting up ScanConfig...");
                ScanConfig config;
                config.targetExtensions = {ext};
                config.deepScan = deepScan_;
                config.threadCount = threads_;
                config.outputDirectory = outputPath;
                LOG_DEBUG("ScanConfig configured");
                
                // Start scan
                LOG_DEBUG("About to call scanner->startScan()...");
                
                if (scanner_->startScan(diskInfo, config)) {
                    std::cout << "[INFO] Scan started successfully, monitoring progress..." << std::endl;
                    
                    // Wait for completion or stop request
                    int loopCount = 0;
                    while (scanner_->isScanning() && !stopRequested_) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        
                        // Safety check: log every 10 seconds
                        if (++loopCount % 100 == 0) {
                            std::cout << "[HEARTBEAT] Still scanning... Files found: " 
                                     << filesFound_.load() << std::endl;
                        }
                        
                        // Log progress for large scans
                        if (recoveredFiles_.size() % 5000 == 0 && recoveredFiles_.size() > 0) {
                            LOG_INFO("Processing: " + std::to_string(recoveredFiles_.size()) + " files in memory");
                        }
                    }
                
                    if (stopRequested_) {
                        scanner_->stopScan();
                    }
                }
            }
            
            std::cout << "\n✅ Recovery complete!" << std::endl;
            std::cout << "📊 Total files found: " << filesFound_.load() << std::endl;
            std::cout << "📁 Saved to: " << outputPath << std::endl;
            
            LOG_INFO("=== Recovery Complete ===");
            LOG_INFO("Total files found: " + std::to_string(filesFound_.load()));
            
        } catch (const std::exception& e) {
            std::cerr << "\n❌ Recovery error: " << e.what() << std::endl;
            LOG_ERROR(std::string("Recovery error: ") + e.what());
        } catch (...) {
            std::cerr << "\n❌ Unknown recovery error!" << std::endl;
            LOG_ERROR("Unknown recovery error");
        }
        
        isRecovering_ = false;
        scanner_.reset();
        recoveryManager_.reset();
    });
}

void RecoveryGUI::stopRecovery() {
    if (!isRecovering_) return;
    
    std::cout << "\n⏹️ Stop requested..." << std::endl;
    stopRequested_ = true;
    
    if (scanner_) {
        scanner_->stopScan();
    }
    
    isRecovering_ = false;
}

void RecoveryGUI::shutdown() {
    if (g_Window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        glfwDestroyWindow(g_Window);
        glfwTerminate();
        
        g_Window = nullptr;
    }
}

void RecoveryGUI::detectAvailableDisks() {
    availableDisks_.clear();
    
#ifdef _WIN32
    // Detect Windows drives
    DWORD drives = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; letter++) {
        if (drives & (1 << (letter - 'A'))) {
            std::string drive = std::string(1, letter) + ":";
            
            // Get drive type
            std::string drivePath = drive + "\\";
            UINT driveType = GetDriveTypeA(drivePath.c_str());
            
            std::string driveInfo = drive;
            switch (driveType) {
                case DRIVE_FIXED:
                    driveInfo += " - Local Disk";
                    break;
                case DRIVE_REMOVABLE:
                    driveInfo += " - Removable Disk";
                    break;
                case DRIVE_REMOTE:
                    driveInfo += " - Network Drive";
                    break;
                case DRIVE_CDROM:
                    driveInfo += " - CD/DVD";
                    break;
                default:
                    driveInfo += " - Unknown";
            }
            
            // Get volume name
            char volumeName[MAX_PATH];
            if (GetVolumeInformationA(drivePath.c_str(), volumeName, MAX_PATH, 
                                     nullptr, nullptr, nullptr, nullptr, 0)) {
                if (volumeName[0] != '\0') {
                    driveInfo += " (" + std::string(volumeName) + ")";
                }
            }
            
            availableDisks_.push_back(driveInfo);
        }
    }
#else
    // Linux/Mac: scan /dev or /Volumes
    availableDisks_.push_back("/dev/sda - Primary Disk");
    availableDisks_.push_back("/dev/sdb - Secondary Disk");
#endif

    if (availableDisks_.empty()) {
        availableDisks_.push_back("No disks detected");
    }
}

void RecoveryGUI::initializeExtensions() {
    extensions_ = {
        {"sav", "Game Save Files (Unreal Engine GVAS)", true},
        {"jpg", "JPEG Images", false},
        {"jpeg", "JPEG Images (alternate)", false},
        {"png", "PNG Images", false},
        {"gif", "GIF Images", false},
        {"bmp", "Bitmap Images", false},
        {"pdf", "PDF Documents", false},
        {"doc", "Microsoft Word Documents", false},
        {"docx", "Microsoft Word Documents (Office 2007+)", false},
        {"xls", "Microsoft Excel Spreadsheets", false},
        {"xlsx", "Microsoft Excel Spreadsheets (Office 2007+)", false},
        {"ppt", "Microsoft PowerPoint Presentations", false},
        {"pptx", "Microsoft PowerPoint Presentations (Office 2007+)", false},
        {"txt", "Text Files", false},
        {"zip", "ZIP Archives", false},
        {"rar", "RAR Archives", false},
        {"7z", "7-Zip Archives", false},
        {"mp3", "MP3 Audio", false},
        {"mp4", "MP4 Video", false},
        {"avi", "AVI Video", false},
        {"mkv", "MKV Video", false},
        {"mov", "QuickTime Video", false},
        {"wav", "WAV Audio", false},
        {"flac", "FLAC Audio", false},
        {"exe", "Executable Files", false},
        {"dll", "Dynamic Link Libraries", false},
        {"iso", "ISO Disk Images", false},
        {"sql", "SQL Database Files", false},
        {"db", "Database Files", false},
        {"psd", "Adobe Photoshop Documents", false},
        {"ai", "Adobe Illustrator Documents", false},
    };
}

void RecoveryGUI::renderPreviewPanel() {
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("File Preview", &showPreview_)) {
        if (selectedFileIndex_ >= 0 && selectedFileIndex_ < static_cast<int>(recoveredFiles_.size())) {
            const auto& selectedFile = recoveredFiles_[selectedFileIndex_];

            ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "File: %s", selectedFile.filename.c_str());
            ImGui::Text("Size: %.2f MB", selectedFile.size / (1024.0 * 1024.0));
            ImGui::Text("Path: %s", selectedFile.path.c_str());

            ImGui::Separator();
            ImGui::Spacing();

            // Display preview based on type
            if (currentPreview_.type == Utils::PreviewType::IMAGE) {
                ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "IMAGE PREVIEW");
                ImGui::Separator();

                if (currentPreview_.error.empty()) {
                    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "✓ Successfully decoded image using stb_image!");
                    ImGui::Text("Original Dimensions: %dx%d", currentPreview_.width, currentPreview_.height);
                    ImGui::Text("Thumbnail Size: %dx%d", currentPreview_.thumbnailWidth, currentPreview_.thumbnailHeight);
                    ImGui::Text("Thumbnail Data: %zu bytes (RGBA)", currentPreview_.thumbnailData.size());
                    ImGui::Spacing();
                    ImGui::TextWrapped("Note: Image successfully decoded. Thumbnail data is ready. OpenGL texture display will be added in next update.");
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "✗ Failed to decode image");
                    ImGui::TextWrapped("Error: %s", currentPreview_.error.c_str());
                }

            } else if (currentPreview_.type == Utils::PreviewType::TEXT) {
                ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "TEXT PREVIEW");
                ImGui::Separator();
                ImGui::Text("Showing %zu of %zu lines", currentPreview_.textLines.size(), currentPreview_.totalLines);
                ImGui::Spacing();

                ImGui::BeginChild("TextContent", ImVec2(0, -30), true, ImGuiWindowFlags_HorizontalScrollbar);
                for (const auto& line : currentPreview_.textLines) {
                    ImGui::TextUnformatted(line.c_str());
                }
                ImGui::EndChild();

            } else if (currentPreview_.type == Utils::PreviewType::HEX) {
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "HEX PREVIEW");
                ImGui::Separator();
                ImGui::Text("Showing first %zu bytes", currentPreview_.hexData.size());
                ImGui::Spacing();

                ImGui::BeginChild("HexContent", ImVec2(0, -30), true, ImGuiWindowFlags_HorizontalScrollbar);

                // Format hex dump nicely
                for (size_t i = 0; i < currentPreview_.hexData.size(); i += 16) {
                    // Offset
                    ImGui::Text("%08zX:", i);
                    ImGui::SameLine();

                    // Hex bytes
                    for (size_t j = 0; j < 16 && (i + j) < currentPreview_.hexData.size(); ++j) {
                        if (j == 8) ImGui::SameLine(0, 10);
                        ImGui::SameLine();
                        ImGui::Text("%02X", currentPreview_.hexData[i + j]);
                    }

                    // ASCII representation
                    ImGui::SameLine(0, 20);
                    ImGui::Text("|");
                    ImGui::SameLine();
                    for (size_t j = 0; j < 16 && (i + j) < currentPreview_.hexData.size(); ++j) {
                        uint8_t byte = currentPreview_.hexData[i + j];
                        if (byte >= 32 && byte <= 126) {
                            ImGui::Text("%c", byte);
                        } else {
                            ImGui::Text(".");
                        }
                        ImGui::SameLine();
                    }
                    ImGui::Text("|");
                }

                ImGui::EndChild();

            } else {
                ImGui::TextDisabled("No preview available for this file type");
            }

            ImGui::Spacing();
            if (ImGui::Button("Close")) {
                showPreview_ = false;
            }
        }
    }
    ImGui::End();
}

void RecoveryGUI::renderDuplicatePanel() {
    ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Duplicate File Detector", &showDuplicates_)) {
        ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "DUPLICATE FILES DETECTED");
        ImGui::Spacing();

        // Statistics
        size_t totalFiles = 0, uniqueFiles = 0, duplicateFiles = 0, wastedSpace = 0;
        duplicateDetector_->getStatistics(totalFiles, uniqueFiles, duplicateFiles, wastedSpace);

        ImGui::Text("Total Files: %zu", totalFiles);
        ImGui::SameLine(200);
        ImGui::Text("Unique: %zu", uniqueFiles);
        ImGui::SameLine(350);
        ImGui::Text("Duplicates: %zu", duplicateFiles);
        ImGui::SameLine(500);
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Wasted Space: %.2f MB", wastedSpace / (1024.0 * 1024.0));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Actions
        if (duplicateGroups_.empty()) {
            ImGui::TextDisabled("No duplicates found!");
        } else {
            ImGui::Text("Found %zu groups of duplicate files:", duplicateGroups_.size());
            ImGui::Spacing();

            if (ImGui::Button("Delete All Lower-Quality Duplicates")) {
                auto filesToDelete = duplicateDetector_->getFilesToDelete();
                size_t deletedCount = 0;
                for (const auto& filepath : filesToDelete) {
                    try {
                        if (std::filesystem::exists(filepath)) {
                            std::filesystem::remove(filepath);
                            deletedCount++;
                            LOG_INFO("Deleted duplicate: " + filepath);
                        }
                    } catch (const std::exception& e) {
                        LOG_ERROR("Failed to delete " + filepath + ": " + e.what());
                    }
                }
                ImGui::OpenPopup("Deletion Complete");
                LOG_INFO("Deleted " + std::to_string(deletedCount) + " duplicate files");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Automatically delete all lower-quality duplicates, keeping the best version");
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Show duplicate groups
            ImGui::BeginChild("DuplicateGroups", ImVec2(0, -30), true);

            for (size_t g = 0; g < duplicateGroups_.size(); g++) {
                const auto& group = duplicateGroups_[g];

                ImGui::PushID(static_cast<int>(g));
                if (ImGui::CollapsingHeader(("Group " + std::to_string(g + 1) + " - " + std::to_string(group.files.size()) + " duplicates (SHA256: " + group.hash.substr(0, 16) + "...)").c_str())) {
                    ImGui::Indent();

                    for (const auto& file : group.files) {
                        bool isBest = (file.filePath == group.bestFile);

                        if (isBest) {
                            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "✓ KEEP");
                        } else {
                            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "✗ DELETE");
                        }
                        ImGui::SameLine();

                        std::filesystem::path p(file.filePath);
                        ImGui::Text("%s (%.2f KB)", p.filename().string().c_str(), file.fileSize / 1024.0);

                        ImGui::SameLine(600);
                        ImGui::TextDisabled("Quality: %.1f%%", file.quality * 100.0f);

                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Full path: %s\nSize: %zu bytes\nQuality: %.2f%%",
                                file.filePath.c_str(), file.fileSize, file.quality * 100.0f);
                        }
                    }

                    ImGui::Unindent();
                }
                ImGui::PopID();
                ImGui::Spacing();
            }

            ImGui::EndChild();
        }

        // Deletion complete popup
        if (ImGui::BeginPopupModal("Deletion Complete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Duplicate files have been deleted successfully!");
            ImGui::Spacing();
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                // Refresh duplicate analysis
                duplicatesAnalyzed_ = false;
            }
            ImGui::EndPopup();
        }

        ImGui::Spacing();
        if (ImGui::Button("Close")) {
            showDuplicates_ = false;
        }
    }
    ImGui::End();
}

} // namespace GUI
} // namespace FileRecovery
