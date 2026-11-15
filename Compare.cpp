#include "Compare.h"
#include "FileManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

// ============================================================================
// DiffLine Implementation
// ============================================================================

DiffLine::DiffLine(char op, const std::string& lineText, int lineV1, int lineV2)
    : operation(op), text(lineText), lineNumberV1(lineV1), lineNumberV2(lineV2) {}

char DiffLine::getOperation() const {
    return operation;
}

std::string DiffLine::getText() const {
    return text;
}

int DiffLine::getLineNumberV1() const {
    return lineNumberV1;
}

int DiffLine::getLineNumberV2() const {
    return lineNumberV2;
}

// ============================================================================
// Compare Implementation
// ============================================================================

Compare::Compare() {}

Compare::~Compare() {}

// Helper: Splits a multi-line string into individual lines
std::vector<std::string> Compare::splitLines(const std::string& text) const {
    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string line;
    while (std::getline(ss, line)) {
        // Strip carriage returns if present (Windows line endings \r\n)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

// Core Comparison Engine using Longest Common Subsequence (LCS)
bool Compare::compareContents(const std::string& label1, const std::string& content1,
                              const std::string& label2, const std::string& content2) const {
    std::cout << "\n============================================================\n";
    std::cout << "              VERSION COMPARISON (DIFF) REPORT              \n";
    std::cout << "============================================================\n";
    std::cout << " Comparing: [" << label1 << "] vs [" << label2 << "]\n";

    // 1. Check whether they are completely identical
    if (content1 == content2) {
        std::cout << " Status   : IDENTICAL\n";
        std::cout << " Result   : Both versions are completely identical (No differences).\n";
        std::cout << "============================================================\n\n";
        return true;
    }

    std::cout << " Status   : DIFFERENT (Changes detected)\n";

    std::vector<std::string> lines1 = splitLines(content1);
    std::vector<std::string> lines2 = splitLines(content2);

    int n = static_cast<int>(lines1.size());
    int m = static_cast<int>(lines2.size());

    // 2. Build LCS Dynamic Programming table
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (lines1[i - 1] == lines2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }

    // 3. Backtrack from dp[n][m] to reconstruct line additions and removals
    std::vector<DiffLine> diff;
    int i = n, j = m;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && lines1[i - 1] == lines2[j - 1]) {
            diff.push_back(DiffLine(' ', lines1[i - 1], i, j));
            i--;
            j--;
        } else if (j > 0 && (i == 0 || dp[i][j - 1] >= dp[i - 1][j])) {
            diff.push_back(DiffLine('+', lines2[j - 1], -1, j));
            j--;
        } else if (i > 0 && (j == 0 || dp[i][j - 1] < dp[i - 1][j])) {
            diff.push_back(DiffLine('-', lines1[i - 1], i, -1));
            i--;
        }
    }
    std::reverse(diff.begin(), diff.end());

    // 4. Calculate metrics
    int addedCount = 0;
    int removedCount = 0;
    int unchangedCount = 0;

    for (const auto& dl : diff) {
        if (dl.getOperation() == '+') addedCount++;
        else if (dl.getOperation() == '-') removedCount++;
        else unchangedCount++;
    }

    // 5. Display detailed line-by-line diff
    std::cout << "\n[Line-by-Line Changes]:\n";
    std::cout << " Legend: [ ] Unchanged | [+] Added in Version 2 | [-] Removed from Version 1\n";
    std::cout << "------------------------------------------------------------\n";

    for (const auto& dl : diff) {
        if (dl.getOperation() == ' ') {
            std::cout << "     " << dl.getText() << "\n";
        } else if (dl.getOperation() == '+') {
            std::cout << "  +  " << dl.getText() << "\n";
        } else if (dl.getOperation() == '-') {
            std::cout << "  -  " << dl.getText() << "\n";
        }
    }

    // 6. Display clear summary
    std::cout << "------------------------------------------------------------\n";
    std::cout << "Summary:\n";
    std::cout << "  Total lines in " << label1 << " : " << n << "\n";
    std::cout << "  Total lines in " << label2 << " : " << m << "\n";
    std::cout << "  Unchanged lines       : " << unchangedCount << "\n";
    std::cout << "  Added lines      (+)  : " << addedCount << "\n";
    std::cout << "  Removed lines    (-)  : " << removedCount << "\n";
    std::cout << "============================================================\n\n";

    return false;
}

bool Compare::compareVersions(const Version* v1, const Version* v2) const {
    if (v1 == nullptr || v2 == nullptr) {
        std::cerr << "[Error] Cannot compare null Version pointer.\n";
        return false;
    }

    std::string label1 = "Version " + std::to_string(v1->getVersionId()) + " (" + v1->getTimestamp() + ")";
    std::string label2 = "Version " + std::to_string(v2->getVersionId()) + " (" + v2->getTimestamp() + ")";

    return compareContents(label1, v1->getContent(), label2, v2->getContent());
}

bool Compare::compareFileVersions(const VersionHistory& history, int versionId1, int versionId2) const {
    const Version* v1 = history.searchVersionById(versionId1);
    if (v1 == nullptr) {
        std::cerr << "[Error] Version " << versionId1 << " does not exist in history.\n";
        return false;
    }

    const Version* v2 = history.searchVersionById(versionId2);
    if (v2 == nullptr) {
        std::cerr << "[Error] Version " << versionId2 << " does not exist in history.\n";
        return false;
    }

    return compareVersions(v1, v2);
}

bool Compare::compareFiles(const std::string& filePath1, const std::string& filePath2) const {
    FileManager fm;
    if (!fm.fileExists(filePath1)) {
        std::cerr << "[Error] File '" << filePath1 << "' does not exist.\n";
        return false;
    }
    if (!fm.fileExists(filePath2)) {
        std::cerr << "[Error] File '" << filePath2 << "' does not exist.\n";
        return false;
    }

    std::string content1, content2;
    fm.readFile(filePath1, content1);
    fm.readFile(filePath2, content2);

    return compareContents(filePath1, content1, filePath2, content2);
}
