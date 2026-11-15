#ifndef VERSIONHISTORY_H
#define VERSIONHISTORY_H

#include "Version.h"
#include "FileManager.h"
#include <string>

/**
 * Class: VersionHistory
 * ---------------------
 * Custom singly linked list implementation managing chronological version history.
 */
class VersionHistory {
private:
    Version* head;          // Pointer to the first (oldest) version
    Version* tail;          // Pointer to the last (latest) version
    int totalVersions;      // Number of active versions in list
    int nextVersionId;      // Auto-incrementing counter for unique IDs

    // Helper: Generate current formatted timestamp
    static std::string getCurrentTimestamp();

public:
    VersionHistory();
    ~VersionHistory();

    // 1. Add a new version (creates node and appends at tail in O(1))
    Version* addVersion(const std::string& filename, const std::string& content);

    // Overload: Add version with explicit timestamp
    Version* addVersionWithTimestamp(const std::string& filename, const std::string& content, const std::string& timestamp);

    // Overload for Project Persistence: Add version with explicit ID and timestamp
    Version* addVersionExplicit(int id, const std::string& filename, const std::string& content, const std::string& timestamp);

    // 2. Display all versions of a specific file
    void displayVersionsOfFile(const std::string& filename) const;

    // Display all versions in repository history
    void displayAllVersions() const;

    // 3. Search for a specific version by its unique ID
    Version* searchVersionById(int versionId) const;
    Version* findVersionById(int versionId) const;

    // 4. Display / return the latest version (O(1) using tail pointer)
    Version* getLatestVersion() const;
    void displayLatestVersion() const;

    // 5. Delete a specific version node by ID (handles head, middle, and tail deletion)
    bool deleteVersion(int versionId);

    // 6. Restore an older version back into a working file using FileManager
    bool restoreVersion(int versionId, const std::string& targetPath, FileManager& fileManager);

    // 7. Count total versions
    int getTotalVersions() const;
    int countVersionsTraversal() const;

    // Pointer accessors
    Version* getHead() const;
    Version* getTail() const;

    bool isEmpty() const;
    void clear();
};

#endif // VERSIONHISTORY_H
