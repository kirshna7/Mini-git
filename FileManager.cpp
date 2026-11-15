#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cctype>

namespace fs = std::filesystem;

FileManager::FileManager() {}

FileManager::~FileManager() {}

// ============================================================================
// Private Helper Functions
// ============================================================================

bool FileManager::isValidFilename(const std::string& filename) const {
    if (filename.empty()) {
        std::cerr << "[Error] Filename cannot be empty.\n";
        return false;
    }

    // Check if filename contains only whitespace
    bool hasNonSpace = false;
    for (char ch : filename) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            hasNonSpace = true;
            break;
        }
    }
    if (!hasNonSpace) {
        std::cerr << "[Error] Filename cannot consist solely of spaces.\n";
        return false;
    }

    // Check for illegal filename/path characters across Windows and macOS
    const std::string invalidChars = "<>:\"|?*";
    for (char ch : filename) {
        if (invalidChars.find(ch) != std::string::npos) {
            std::cerr << "[Error] Filename contains invalid character '" << ch << "'.\n";
            std::cerr << "        Disallowed characters: < > : \" | ? *\n";
            return false;
        }
    }

    // Prevent relative path traversal sequences
    if (filename == "." || filename == ".." || filename.find("../") != std::string::npos || filename.find("..\\") != std::string::npos) {
        std::cerr << "[Error] Invalid path traversal in filename.\n";
        return false;
    }

    return true;
}

bool FileManager::askConfirmation(const std::string& message) const {
    std::cout << message << " (y/n): ";
    char choice;
    std::cin >> choice;
    // Clear residual characters from standard input stream
    std::cin.ignore(10000, '\n');
    return (choice == 'y' || choice == 'Y');
}

// ============================================================================
// Core FileManager Operations
// ============================================================================

// Operation 5: Check whether a file exists
bool FileManager::fileExists(const std::string& filename) const {
    if (filename.empty()) {
        return false;
    }
    std::ifstream testStream(filename);
    return testStream.good();
}

// Operation 1: Create a new text file
bool FileManager::createFile(const std::string& filename) {
    if (!isValidFilename(filename)) {
        return false;
    }

    // Protection: Never overwrite an existing file without confirmation
    if (fileExists(filename)) {
        std::cout << "[Warning] File '" << filename << "' already exists.\n";
        if (!askConfirmation("Do you want to overwrite it with an empty file?")) {
            std::cout << "[Info] File creation cancelled by user. Original file kept intact.\n";
            return false;
        }
    }

    // Create file using std::ofstream
    std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "[Error] Failed to create file '" << filename << "'. Please check folder permissions.\n";
        return false;
    }

    // Ensure contents are committed to disk
    outFile.flush();
    outFile.close();

    std::cout << "[Success] File '" << filename << "' created successfully.\n";
    return true;
}

// Operation 2: Write content into a file
bool FileManager::writeFile(const std::string& filename, const std::string& content) {
    if (!isValidFilename(filename)) {
        return false;
    }

    // Protection: Confirm before overwriting existing content
    if (fileExists(filename)) {
        std::cout << "[Warning] File '" << filename << "' already exists.\n";
        if (!askConfirmation("Do you want to overwrite its contents?")) {
            std::cout << "[Info] Write operation cancelled. Existing content preserved.\n";
            return false;
        }
    }

    std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "[Error] Could not open file '" << filename << "' for writing.\n";
        return false;
    }

    outFile << content;
    if (outFile.fail()) {
        std::cerr << "[Error] An I/O error occurred while writing to '" << filename << "'.\n";
        outFile.close();
        return false;
    }

    // Operation 6: Flush buffer to guarantee permanent storage on disk
    outFile.flush();
    outFile.close();

    std::cout << "[Success] Content written and saved to '" << filename << "'.\n";
    return true;
}

// Operation 3: Read and display file content
bool FileManager::readAndDisplayFile(const std::string& filename) const {
    if (!isValidFilename(filename)) {
        return false;
    }

    // Handle missing file
    if (!fileExists(filename)) {
        std::cerr << "[Error] Cannot read file '" << filename << "'. File does not exist.\n";
        return false;
    }

    std::ifstream inFile(filename, std::ios::in);
    if (!inFile.is_open()) {
        std::cerr << "[Error] Failed to open file '" << filename << "' for reading.\n";
        return false;
    }

    // Handle empty file
    if (inFile.peek() == std::ifstream::traits_type::eof()) {
        std::cout << "[Notice] File '" << filename << "' is empty (0 bytes).\n";
        inFile.close();
        return true;
    }

    std::cout << "\n================ Content of " << filename << " ================\n";
    std::string line;
    int lineNumber = 1;
    while (std::getline(inFile, line)) {
        std::cout << lineNumber++ << " | " << line << "\n";
    }
    std::cout << "======================================================\n\n";

    inFile.close();
    return true;
}

// Operation 4: Edit existing file content using fstream
bool FileManager::editFile(const std::string& filename, const std::string& newContent, bool appendMode) {
    if (!isValidFilename(filename)) {
        return false;
    }

    // Handle missing file
    if (!fileExists(filename)) {
        std::cerr << "[Error] Cannot edit '" << filename << "'. File does not exist.\n";
        return false;
    }

    if (appendMode) {
        // Use fstream in append mode (std::ios::in | std::ios::out | std::ios::app)
        std::fstream fileStream(filename, std::ios::in | std::ios::out | std::ios::app);
        if (!fileStream.is_open()) {
            std::cerr << "[Error] Failed to open '" << filename << "' for appending.\n";
            return false;
        }

        // Add a newline if content doesn't end with one, then append
        fileStream << newContent;
        if (!newContent.empty() && newContent.back() != '\n') {
            fileStream << "\n";
        }

        // Operation 6: Flush buffer to commit changes permanently
        fileStream.flush();
        fileStream.close();

        std::cout << "[Success] Content appended to '" << filename << "'.\n";
        return true;
    } else {
        // Replacement mode: confirm before replacing all contents
        std::cout << "[Warning] You are about to replace all existing content in '" << filename << "'.\n";
        if (!askConfirmation("Are you sure you want to proceed?")) {
            std::cout << "[Info] Edit cancelled. Existing content preserved.\n";
            return false;
        }

        std::fstream fileStream(filename, std::ios::in | std::ios::out | std::ios::trunc);
        if (!fileStream.is_open()) {
            std::cerr << "[Error] Failed to open '" << filename << "' for editing.\n";
            return false;
        }

        fileStream << newContent;
        fileStream.flush();
        fileStream.close();

        std::cout << "[Success] Content replaced in '" << filename << "'.\n";
        return true;
    }
}

// Operation 6: Save file content permanently (convenience method)
bool FileManager::saveFile(const std::string& filename, const std::string& content) {
    if (!isValidFilename(filename)) {
        return false;
    }

    std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "[Error] Failed to open '" << filename << "' for saving.\n";
        return false;
    }

    outFile << content;
    outFile.flush();  // Explicit flush commits to persistent storage
    outFile.close();

    std::cout << "[Success] File '" << filename << "' permanently saved.\n";
    return true;
}

// ============================================================================
// Utility Methods (for commit snapshots & directory management)
// ============================================================================

bool FileManager::readFile(const std::string& filename, std::string& outContent) const {
    outContent.clear();
    if (!fileExists(filename)) {
        return false;
    }

    std::ifstream inFile(filename, std::ios::in);
    if (!inFile.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        outContent += line + "\n";
    }

    inFile.close();
    return true;
}

bool FileManager::copyFile(const std::string& source, const std::string& destination) {
    if (!fileExists(source)) {
        std::cerr << "[Error] Source file '" << source << "' does not exist.\n";
        return false;
    }

    // Use binary mode for robust cross-platform copying
    std::ifstream srcFile(source, std::ios::in | std::ios::binary);
    if (!srcFile.is_open()) {
        std::cerr << "[Error] Could not open source file '" << source << "'.\n";
        return false;
    }

    std::ofstream destFile(destination, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!destFile.is_open()) {
        std::cerr << "[Error] Could not open destination file '" << destination << "'.\n";
        srcFile.close();
        return false;
    }

    // Stream buffer copy
    destFile << srcFile.rdbuf();

    srcFile.close();
    destFile.flush();
    destFile.close();

    return true;
}

bool FileManager::createDirectory(const std::string& path) {
    try {
        if (!fs::exists(path)) {
            return fs::create_directories(path);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Error] Failed to create directory: " << e.what() << "\n";
        return false;
    }
}
