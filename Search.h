#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <vector>
#include "Version.h"
#include "VersionHistory.h"

/**
 * Class: Search
 * -------------
 * Provides custom searching and sorting algorithms for Mini Git.
 *
 * SEARCHING ALGORITHM: Linear Search (O(N))
 * - Singly linked lists lack random access; linear traversal is optimal.
 *
 * SORTING ALGORITHM: Bubble Sort on Singly Linked List (O(N^2))
 * - Iteratively compares adjacent nodes and swaps payload data.
 * - Guarantees 0 broken pointers and 0 lost versions.
 */
class Search {
public:
    Search();
    ~Search();

    // ========================================================================
    // Searching Operations (Linear Search)
    // ========================================================================

    // 1. Search for a specific Version by its unique ID
    Version* searchById(const VersionHistory& history, int versionId) const;

    // 2. Search all versions matching a specific filename
    std::vector<Version*> searchByFilename(const VersionHistory& history, const std::string& filename) const;

    // 3. Search for versions containing a keyword in their content
    std::vector<Version*> searchByKeyword(const VersionHistory& history, const std::string& keyword) const;

    // ========================================================================
    // Sorting Operations (Bubble Sort on Linked List)
    // ========================================================================

    // 1. Sort the linked list by Version ID (Ascending or Descending)
    void sortById(VersionHistory& history, bool ascending = true) const;

    // 2. Sort the linked list by Timestamp (Ascending or Descending)
    void sortByTimestamp(VersionHistory& history, bool ascending = true) const;

    // 3. Display versions in specified sorted order without mutating original history order
    void displaySorted(const VersionHistory& history, const std::string& criteria, bool ascending) const;
};

#endif // SEARCH_H
