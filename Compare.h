#ifndef COMPARE_H
#define COMPARE_H

#include <string>
#include <vector>
#include "Version.h"
#include "VersionHistory.h"

// Struct-free diff entry representation (using class)
class DiffLine {
private:
    char operation;            // ' ' (Unchanged), '+' (Added), '-' (Removed)
    std::string text;
    int lineNumberV1;
    int lineNumberV2;

public:
    DiffLine(char op, const std::string& lineText, int lineV1 = -1, int lineV2 = -1);

    char getOperation() const;
    std::string getText() const;
    int getLineNumberV1() const;
    int getLineNumberV2() const;
};

/**
 * Class: Compare
 * --------------
 * Compares two versions of a file using Longest Common Subsequence (LCS) line diff.
 *
 * Capabilities:
 * 1. Reads contents of both versions.
 * 2. Compares line-by-line without external libraries.
 * 3. Identifies whether versions are identical.
 * 4. Identifies added and removed lines.
 * 5. Generates a formatted comparison report.
 */
class Compare {
private:
    // Helper to split a multi-line string into a vector of lines
    std::vector<std::string> splitLines(const std::string& text) const;

public:
    Compare();
    ~Compare();

    // Compares two Version objects directly
    bool compareVersions(const Version* v1, const Version* v2) const;

    // Compares two versions retrieved by ID from VersionHistory
    bool compareFileVersions(const VersionHistory& history, int versionId1, int versionId2) const;

    // Compares two raw text contents with descriptive labels
    bool compareContents(const std::string& label1, const std::string& content1,
                         const std::string& label2, const std::string& content2) const;

    // Compares two physical files on disk
    bool compareFiles(const std::string& filePath1, const std::string& filePath2) const;
};

#endif // COMPARE_H
