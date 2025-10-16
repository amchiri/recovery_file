#include "BatchOperations.h"
#include "RecoveryGUI.h"
#include <imgui.h>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <windows.h>
#include <shellapi.h>

namespace fs = std::filesystem;

namespace FileRecovery {

void BatchOperations::toggleSelection(size_t index) {
    if (selectedFiles_.count(index)) {
        selectedFiles_.erase(index);
    } else {
        selectedFiles_.insert(index);
    }
}

void BatchOperations::selectAll(size_t totalFiles) {
    selectedFiles_.clear();
    for (size_t i = 0; i < totalFiles; i++) {
        selectedFiles_.insert(i);
    }
}

void BatchOperations::deselectAll() {
    selectedFiles_.clear();
}

void BatchOperations::selectRange(size_t startIndex, size_t endIndex) {
    for (size_t i = startIndex; i <= endIndex; i++) {
        selectedFiles_.insert(i);
    }
}

bool BatchOperations::isSelected(size_t index) const {
    return selectedFiles_.count(index) > 0;
}

std::vector<size_t> BatchOperations::getSelectedIndices() const {
    std::vector<size_t> indices(selectedFiles_.begin(), selectedFiles_.end());
    std::sort(indices.begin(), indices.end());
    return indices;
}

void BatchOperations::renderBatchControls(size_t totalFiles) {
    size_t selected = getSelectedCount();
    
    // Selection controls
    if (ImGui::Button("Select All")) {
        selectAll(totalFiles);
    }
    ImGui::SameLine();
    if (ImGui::Button("Deselect All")) {
        deselectAll();
    }
    ImGui::SameLine();
    
    // Show selected count with color
    if (selected > 0) {
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "Selected: %zu file%s", 
                          selected, selected > 1 ? "s" : "");
    } else {
        ImGui::TextDisabled("Selected: 0 files");
    }
    
    ImGui::Spacing();
    
    // Batch operation buttons
    if (selected > 0) {
        // Delete Selected
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button(("Delete Selected (" + std::to_string(selected) + ")").c_str())) {
            ImGui::OpenPopup("Confirm Delete");
        }
        ImGui::PopStyleColor(2);
        
        // Delete confirmation popup
        if (ImGui::BeginPopupModal("Confirm Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "WARNING");
            ImGui::Text("Are you sure you want to delete %zu file%s?", 
                       selected, selected > 1 ? "s" : "");
            ImGui::Text("This action cannot be undone!");
            ImGui::Spacing();
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("Yes, Delete", ImVec2(120, 0))) {
                // Mark for deletion (will be handled in RecoveryGUI)
                ImGui::CloseCurrentPopup();
                // Signal deletion confirmation
            }
            ImGui::PopStyleColor();
            
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        
        ImGui::SameLine();
        
        // Copy Selected To...
        if (ImGui::Button(("Copy Selected To... (" + std::to_string(selected) + ")").c_str())) {
            ImGui::OpenPopup("Copy Files");
        }
        
        // Copy popup (simplified - full folder picker would need platform-specific code)
        if (ImGui::BeginPopupModal("Copy Files", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Copy %zu file%s to:", selected, selected > 1 ? "s" : "");
            ImGui::Spacing();
            
            static char destPath[512] = "";
            ImGui::InputText("Destination", destPath, sizeof(destPath));
            ImGui::Spacing();
            
            if (ImGui::Button("Copy", ImVec2(120, 0))) {
                // Will be handled in RecoveryGUI
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        
        ImGui::SameLine();
        
        // Open All
        if (ImGui::Button(("Open All (" + std::to_string(selected) + ")").c_str())) {
            ImGui::OpenPopup("Confirm Open All");
        }
        
        // Open all confirmation
        if (ImGui::BeginPopupModal("Confirm Open All", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Open %zu file%s in their default applications?", 
                       selected, selected > 1 ? "s" : "");
            if (selected > 10) {
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), 
                                 "Warning: This will open many files at once!");
            }
            ImGui::Spacing();
            
            if (ImGui::Button("Yes, Open", ImVec2(120, 0))) {
                // Will be handled in RecoveryGUI
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        
    } else {
        // Disabled buttons when nothing selected
        ImGui::BeginDisabled();
        ImGui::Button("Delete Selected (0)");
        ImGui::SameLine();
        ImGui::Button("Copy Selected To... (0)");
        ImGui::SameLine();
        ImGui::Button("Open All (0)");
        ImGui::EndDisabled();
    }
}

bool BatchOperations::deleteSelected(const std::vector<GUI::RecoveredFileInfo>& files) {
    bool allSuccess = true;
    std::vector<size_t> indices = getSelectedIndices();
    
    for (size_t idx : indices) {
        if (idx >= files.size()) continue;
        
        const auto& file = files[idx];
        try {
            if (fs::exists(file.path)) {
                fs::remove(file.path);
                std::cout << "✅ Deleted: " << file.path << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Failed to delete " << file.path << ": " << e.what() << std::endl;
            allSuccess = false;
        }
    }
    
    // Clear selection after delete
    clearSelection();
    
    return allSuccess;
}

bool BatchOperations::copySelectedTo(const std::vector<GUI::RecoveredFileInfo>& files, const std::string& destPath) {
    if (destPath.empty() || !fs::exists(destPath)) {
        std::cerr << "❌ Invalid destination path" << std::endl;
        return false;
    }
    
    bool allSuccess = true;
    std::vector<size_t> indices = getSelectedIndices();
    
    for (size_t idx : indices) {
        if (idx >= files.size()) continue;
        
        const auto& file = files[idx];
        try {
            if (fs::exists(file.path)) {
                fs::path dest = fs::path(destPath) / fs::path(file.path).filename();
                fs::copy_file(file.path, dest, fs::copy_options::overwrite_existing);
                std::cout << "✅ Copied: " << file.path << " -> " << dest << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Failed to copy " << file.path << ": " << e.what() << std::endl;
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool BatchOperations::openSelected(const std::vector<GUI::RecoveredFileInfo>& files) {
    bool allSuccess = true;
    std::vector<size_t> indices = getSelectedIndices();
    
    for (size_t idx : indices) {
        if (idx >= files.size()) continue;
        
        const auto& file = files[idx];
        try {
            if (fs::exists(file.path)) {
                // Use ShellExecute to open file with default application
                HINSTANCE result = ShellExecuteA(nullptr, "open", 
                                                file.path.c_str(), 
                                                nullptr, nullptr, SW_SHOW);
                
                if ((INT_PTR)result > 32) {
                    std::cout << "✅ Opened: " << file.path << std::endl;
                } else {
                    std::cerr << "❌ Failed to open: " << file.path << std::endl;
                    allSuccess = false;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Error opening " << file.path << ": " << e.what() << std::endl;
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

} // namespace FileRecovery
