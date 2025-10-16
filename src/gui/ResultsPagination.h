#pragma once
#include <cstddef>

namespace FileRecovery {

class ResultsPagination {
public:
    ResultsPagination(size_t itemsPerPage = 100);
    
    void setTotalItems(size_t total);
    void render(); // Affiche les contrôles de pagination
    
    size_t getCurrentPage() const { return currentPage_; }
    size_t getStartIndex() const { return currentPage_ * itemsPerPage_; }
    size_t getEndIndex() const;
    size_t getTotalPages() const;
    
    void nextPage();
    void prevPage();
    void goToPage(size_t page);
    void setItemsPerPage(size_t itemsPerPage);
    
private:
    size_t currentPage_ = 0;
    size_t itemsPerPage_ = 100;
    size_t totalItems_ = 0;
};

} // namespace FileRecovery
