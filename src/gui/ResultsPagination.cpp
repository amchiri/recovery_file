#include "ResultsPagination.h"
#include <imgui.h>
#include <algorithm>
#include <string>

namespace FileRecovery {

ResultsPagination::ResultsPagination(size_t itemsPerPage) 
    : itemsPerPage_(itemsPerPage) {}

void ResultsPagination::setTotalItems(size_t total) {
    totalItems_ = total;
    // Ensure current page is still valid
    if (getTotalPages() > 0) {
        currentPage_ = std::min(currentPage_, getTotalPages() - 1);
    } else {
        currentPage_ = 0;
    }
}

size_t ResultsPagination::getEndIndex() const {
    return std::min(getStartIndex() + itemsPerPage_, totalItems_);
}

size_t ResultsPagination::getTotalPages() const {
    if (totalItems_ == 0) return 1;
    return (totalItems_ + itemsPerPage_ - 1) / itemsPerPage_;
}

void ResultsPagination::nextPage() {
    if (currentPage_ < getTotalPages() - 1) {
        currentPage_++;
    }
}

void ResultsPagination::prevPage() {
    if (currentPage_ > 0) {
        currentPage_--;
    }
}

void ResultsPagination::goToPage(size_t page) {
    if (page < getTotalPages()) {
        currentPage_ = page;
    }
}

void ResultsPagination::setItemsPerPage(size_t itemsPerPage) {
    if (itemsPerPage > 0) {
        itemsPerPage_ = itemsPerPage;
        // Recalculate current page to stay in valid range
        if (getTotalPages() > 0) {
            currentPage_ = std::min(currentPage_, getTotalPages() - 1);
        }
    }
}

void ResultsPagination::render() {
    size_t totalPages = getTotalPages();
    
    if (totalItems_ == 0) {
        ImGui::TextDisabled("No items to display");
        return;
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Show items range
    size_t start = getStartIndex() + 1; // 1-based for display
    size_t end = getEndIndex();
    ImGui::Text("Showing %zu - %zu of %zu files", start, end, totalItems_);
    
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    
    // Previous button
    if (currentPage_ == 0) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("<< Previous")) {
        prevPage();
    }
    if (currentPage_ == 0) {
        ImGui::EndDisabled();
    }
    
    ImGui::SameLine();
    
    // Page numbers (show max 7 pages)
    size_t maxPagesToShow = 7;
    size_t startPage = 0;
    size_t endPage = totalPages;
    
    if (totalPages > maxPagesToShow) {
        // Smart pagination: show pages around current page
        if (currentPage_ < maxPagesToShow / 2) {
            // Near start
            startPage = 0;
            endPage = maxPagesToShow;
        } else if (currentPage_ >= totalPages - maxPagesToShow / 2) {
            // Near end
            startPage = totalPages - maxPagesToShow;
            endPage = totalPages;
        } else {
            // Middle
            startPage = currentPage_ - maxPagesToShow / 2;
            endPage = currentPage_ + maxPagesToShow / 2 + 1;
        }
    }
    
    // Show first page if not in range
    if (startPage > 0) {
        if (ImGui::Button("1")) {
            goToPage(0);
        }
        ImGui::SameLine();
        ImGui::TextDisabled("...");
        ImGui::SameLine();
    }
    
    // Show page numbers
    for (size_t i = startPage; i < endPage; i++) {
        if (i == currentPage_) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
        }
        
        std::string label = std::to_string(i + 1);
        if (ImGui::Button(label.c_str())) {
            goToPage(i);
        }
        
        if (i == currentPage_) {
            ImGui::PopStyleColor(2);
        }
        
        if (i < endPage - 1) {
            ImGui::SameLine();
        }
    }
    
    // Show last page if not in range
    if (endPage < totalPages) {
        ImGui::SameLine();
        ImGui::TextDisabled("...");
        ImGui::SameLine();
        std::string lastLabel = std::to_string(totalPages);
        if (ImGui::Button(lastLabel.c_str())) {
            goToPage(totalPages - 1);
        }
    }
    
    ImGui::SameLine();
    
    // Next button
    if (currentPage_ >= totalPages - 1) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Next >>")) {
        nextPage();
    }
    if (currentPage_ >= totalPages - 1) {
        ImGui::EndDisabled();
    }
    
    ImGui::SameLine();
    ImGui::Text("(Page %zu / %zu)", currentPage_ + 1, totalPages);
    
    ImGui::Spacing();
}

} // namespace FileRecovery
