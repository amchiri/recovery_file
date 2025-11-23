#include "utils/ConfigManager.h"
#include "utils/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace FileRecovery {
namespace Utils {

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

std::string ConfigManager::makeKey(const std::string& section, const std::string& key) const {
    return section + "." + key;
}

bool ConfigManager::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log(LogLevel::WARNING,
            "Config file not found: " + filename + ", using defaults");
        return false;
    }

    config_.clear();
    configFile_ = filename;

    std::string currentSection;
    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        lineNum++;

        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Section header [SectionName]
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }

        // Key=Value pair
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);

            // Trim key and value
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));

            // Remove quotes from value if present
            if (value.length() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }

            config_[makeKey(currentSection, key)] = value;
        }
    }

    file.close();
    Logger::getInstance().log(LogLevel::INFO,
        "Loaded configuration from " + filename + " (" + std::to_string(config_.size()) + " settings)");
    return true;
}

bool ConfigManager::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to save config to: " + filename);
        return false;
    }

    // Group by section
    std::map<std::string, std::map<std::string, std::string>> sections;
    for (const auto& [fullKey, value] : config_) {
        size_t dotPos = fullKey.find('.');
        if (dotPos != std::string::npos) {
            std::string section = fullKey.substr(0, dotPos);
            std::string key = fullKey.substr(dotPos + 1);
            sections[section][key] = value;
        }
    }

    // Write file
    file << "# File Recovery Tool Configuration\n";
    file << "# Auto-generated - Edit with caution\n\n";

    for (const auto& [section, keys] : sections) {
        file << "[" << section << "]\n";
        for (const auto& [key, value] : keys) {
            file << key << " = " << value << "\n";
        }
        file << "\n";
    }

    file.close();
    Logger::getInstance().log(LogLevel::INFO, "Saved configuration to: " + filename);
    return true;
}

std::string ConfigManager::getString(const std::string& section, const std::string& key,
                                     const std::string& defaultValue) const {
    auto it = config_.find(makeKey(section, key));
    return (it != config_.end()) ? it->second : defaultValue;
}

int ConfigManager::getInt(const std::string& section, const std::string& key, int defaultValue) const {
    auto it = config_.find(makeKey(section, key));
    if (it != config_.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& section, const std::string& key, bool defaultValue) const {
    auto it = config_.find(makeKey(section, key));
    if (it != config_.end()) {
        std::string val = it->second;
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        return (val == "true" || val == "1" || val == "yes" || val == "on");
    }
    return defaultValue;
}

void ConfigManager::setString(const std::string& section, const std::string& key, const std::string& value) {
    config_[makeKey(section, key)] = value;
}

void ConfigManager::setInt(const std::string& section, const std::string& key, int value) {
    config_[makeKey(section, key)] = std::to_string(value);
}

void ConfigManager::setBool(const std::string& section, const std::string& key, bool value) {
    config_[makeKey(section, key)] = value ? "true" : "false";
}

void ConfigManager::createDefaultConfig(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }

    file << R"(# File Recovery Tool Configuration
# This file is automatically loaded at startup
# Edit values to customize default behavior

[Paths]
# Default output directory for recovered files
output_directory = .\recovered_files

# Default scan folder (leave empty for full disk scan)
scan_folder =

# Signature database file (optional, falls back to built-in signatures)
signature_file = signatures.txt

[Performance]
# Number of threads for scanning (0 = auto-detect)
thread_count = 4

# Enable deep scan by default
deep_scan = true

# Enable signature verification
verify_signatures = true

# Buffer size for disk I/O in MB (higher = faster but more RAM)
buffer_size_mb = 16

[Scanning]
# Enable smart scan optimization (skip empty sectors)
smart_scan = true

# Enable large buffer optimization
large_buffers = true

# Scan specific folder instead of entire disk
scan_specific_folder = false

[FileTypes]
# Default file types to recover (comma-separated extensions)
# Leave empty to recover all types
default_types = jpg,png,pdf,docx,mp4,zip

[GUI]
# Window width
window_width = 1280

# Window height
window_height = 720

# Enable dark theme
dark_theme = true

# Show file preview (not implemented yet)
show_preview = false

[Logging]
# Log level: DEBUG, INFO, WARNING, ERROR
log_level = INFO

# Log file path
log_file = recovery_log.txt

# Enable console output
console_output = true
)";

    file.close();
}

} // namespace Utils
} // namespace FileRecovery
