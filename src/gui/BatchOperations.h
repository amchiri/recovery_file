#pragma once
#include <unordered_set>
#include <vector>
#include <cstddef>
#include <string>

namespace FileRecovery {
namespace GUI {
    struct RecoveredFileInfo; // Forward declaration
}

class BatchOperations {
public:
    BatchOperations() = default;
    
    // Selection management
    void toggleSelection(size_t index);
    void selectAll(size_t totalFiles);
    void deselectAll();
    void selectRange(size_t startIndex, size_t endIndex);
    
    bool isSelected(size_t index) const;
    size_t getSelectedCount() const { return selectedFiles_.size(); }
    std::vector<size_t> getSelectedIndices() const;
    
    // Render UI
    void renderBatchControls(size_t totalFiles);
    
    // Batch operations
    bool deleteSelected(const std::vector<GUI::RecoveredFileInfo>& files);
    bool copySelectedTo(const std::vector<GUI::RecoveredFileInfo>& files, const std::string& destPath);
    bool openSelected(const std::vector<GUI::RecoveredFileInfo>& files);
    
    // State
    void clearSelection() { selectedFiles_.clear(); }
    bool hasSelection() const { return !selectedFiles_.empty(); }
    
private:
    std::unordered_set<size_t> selectedFiles_;
    
    // Helper for confirmation dialogs
    bool showConfirmDialog(const char* title, const char* message);
};

} // namespace FileRecovery
