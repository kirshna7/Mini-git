#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>

/**
 * Class: FileManager
 * -------------------
 * Handles all fundamental file system operations for Mini Git.
 * Uses standard C++ file streams:
 *   - std::ifstream (reading files)
 *   - std::ofstream (creating, writing, and saving files)
 *   - std::fstream  (reading and editing existing files)
 */
class FileManager {
private:
    // Helper to validate filenames against invalid characters and empty strings
    bool isValidFilename(const std::string& filename) const;

    // Helper to ask user confirmation in console before dangerous operations
    bool askConfirmation(const std::string& message) const;

public:
    FileManager();
    ~FileManager();

    // 1. Create a new text file safely (checks for existence and confirms before overwriting)
    bool createFile(const std::string& filename);

    // 2. Write content into a file (prompts for confirmation if file already exists)
    bool writeFile(const std::string& filename, const std::string& content);

    // 3. Read and display file content (handles missing files and empty files)
    bool readAndDisplayFile(const std::string& filename) const;

    // 4. Edit an existing file (append new content or replace content using fstream)
    bool editFile(const std::string& filename, const std::string& newContent, bool appendMode = true);

    // 5. Check whether a file exists on disk
    bool fileExists(const std::string& filename) const;

    // 6. Save content permanently to disk with explicit stream flush and close
    bool saveFile(const std::string& filename, const std::string& content);

    // Utility: Read full content into a string buffer
    bool readFile(const std::string& filename, std::string& outContent) const;

    // Utility: Copy file from source to destination (for commit snapshots)
    bool copyFile(const std::string& source, const std::string& destination);

    // Utility: Create directory (cross-platform using C++17 filesystem)
    bool createDirectory(const std::string& path);
};

#endif // FILEMANAGER_H
