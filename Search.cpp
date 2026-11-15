#include "Search.h"
#include <iostream>

Search::Search() {}

Search::~Search() {}

// ============================================================================
// Searching Operations (Linear Search: O(N))
// ============================================================================

// 1. Search for a specific Version by ID
Version* Search::searchById(const VersionHistory& history, int versionId) const {
    Version* current = history.getHead();
    while (current != nullptr) {
        if (current->getVersionId() == versionId) {
            return current;
        }
        current = current->getNext();
    }
    return nullptr;
}

// 2. Search for all versions matching a filename
std::vector<Version*> Search::searchByFilename(const VersionHistory& history, const std::string& filename) const {
    std::vector<Version*> matches;
    Version* current = history.getHead();
    while (current != nullptr) {
        if (current->getFilename() == filename) {
            matches.push_back(current);
        }
        current = current->getNext();
    }
    return matches;
}

// 3. Search for versions containing a keyword in their content
std::vector<Version*> Search::searchByKeyword(const VersionHistory& history, const std::string& keyword) const {
    std::vector<Version*> matches;
    if (keyword.empty()) {
        return matches;
    }

    Version* current = history.getHead();
    while (current != nullptr) {
        if (current->getContent().find(keyword) != std::string::npos) {
            matches.push_back(current);
        }
        current = current->getNext();
    }
    return matches;
}

// ============================================================================
// Sorting Operations (Bubble Sort on Singly Linked List: O(N^2))
// ============================================================================

// 1. Sort versions by Version ID in-place
void Search::sortById(VersionHistory& history, bool ascending) const {
    if (history.getHead() == nullptr || history.getHead()->getNext() == nullptr) {
        return; // 0 or 1 elements are already sorted
    }

    bool swapped;
    do {
        swapped = false;
        Version* current = history.getHead();
        while (current != nullptr && current->getNext() != nullptr) {
            bool shouldSwap = false;
            if (ascending) {
                shouldSwap = (current->getVersionId() > current->getNext()->getVersionId());
            } else {
                shouldSwap = (current->getVersionId() < current->getNext()->getVersionId());
            }

            if (shouldSwap) {
                // Swap payload data without mutating next pointers
                current->swapData(current->getNext());
                swapped = true;
            }
            current = current->getNext();
        }
    } while (swapped);
}

// 2. Sort versions by Timestamp in-place
void Search::sortByTimestamp(VersionHistory& history, bool ascending) const {
    if (history.getHead() == nullptr || history.getHead()->getNext() == nullptr) {
        return;
    }

    bool swapped;
    do {
        swapped = false;
        Version* current = history.getHead();
        while (current != nullptr && current->getNext() != nullptr) {
            bool shouldSwap = false;
            if (ascending) {
                shouldSwap = (current->getTimestamp() > current->getNext()->getTimestamp());
            } else {
                shouldSwap = (current->getTimestamp() < current->getNext()->getTimestamp());
            }

            if (shouldSwap) {
                current->swapData(current->getNext());
                swapped = true;
            }
            current = current->getNext();
        }
    } while (swapped);
}

// 3. Display sorted versions without modifying original linked list order
void Search::displaySorted(const VersionHistory& history, const std::string& criteria, bool ascending) const {
    if (history.getHead() == nullptr) {
        std::cout << "[Notice] History is empty. Nothing to display.\n";
        return;
    }

    // Collect pointers to all nodes
    std::vector<Version*> list;
    Version* current = history.getHead();
    while (current != nullptr) {
        list.push_back(current);
        current = current->getNext();
    }

    // Manual Bubble Sort on pointer list (No std::sort used!)
    int n = static_cast<int>(list.size());
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            bool shouldSwap = false;
            if (criteria == "id") {
                shouldSwap = ascending ? (list[j]->getVersionId() > list[j + 1]->getVersionId())
                                       : (list[j]->getVersionId() < list[j + 1]->getVersionId());
            } else { // criteria == "time"
                shouldSwap = ascending ? (list[j]->getTimestamp() > list[j + 1]->getTimestamp())
                                       : (list[j]->getTimestamp() < list[j + 1]->getTimestamp());
            }

            if (shouldSwap) {
                Version* temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }

    // Display formatted sorted view
    std::cout << "\n==================================================\n";
    std::cout << "  SORTED VERSIONS (" << (criteria == "id" ? "By ID" : "By Timestamp")
              << " - " << (ascending ? "ASCENDING" : "DESCENDING") << ")\n";
    std::cout << "==================================================\n";

    for (Version* v : list) {
        v->printDetails();
    }
    std::cout << "Total versions displayed: " << list.size() << "\n";
    std::cout << "==================================================\n\n";
}
