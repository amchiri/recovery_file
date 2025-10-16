#include "gui/RecoveryGUI.h"
#include "gui/Logger.h"
#include <iostream>

int main() {
    // Initialize logger FIRST, before anything else
    try {
        FileLogger::getInstance().init("recovery_log.txt");
        LOG_INFO("========================================");
        LOG_INFO("File Recovery Tool - GUI Started");
        LOG_INFO("========================================");
    } catch (...) {
        std::cerr << "Warning: Failed to initialize logger\n";
    }
    
    try {
        LOG_INFO("Creating RecoveryGUI instance...");
        FileRecovery::GUI::RecoveryGUI gui;
        
        LOG_INFO("Initializing GUI...");
        if (!gui.initialize()) {
            std::cerr << "Failed to initialize GUI\n";
            LOG_ERROR("Failed to initialize GUI");
            return 1;
        }
        
        LOG_INFO("GUI initialized successfully, starting main loop...");
        gui.run();
        
        LOG_INFO("Main loop ended, shutting down...");
        gui.shutdown();
        
        LOG_INFO("GUI shutdown complete");
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        LOG_ERROR(std::string("Exception in main: ") + e.what());
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error\n";
        LOG_ERROR("Unknown exception in main");
        return 1;
    }
}
