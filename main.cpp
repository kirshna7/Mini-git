#include <iostream>
#include <string>
#include <limits>
#include "MiniGit.h"

// ============================================================================
// Safe Input Helper Functions (Prevent Crashes & Handle Edge Cases)
// ============================================================================

int getValidatedInteger(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (value >= minVal && value <= maxVal) {
                return value;
            }
            std::cout << "[Error] Input must be between " << minVal << " and " << maxVal << ". Please try again.\n";
        } else {
            if (std::cin.eof()) {
                // If EOF is reached, exit gracefully rather than infinite loop
                return maxVal;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[Error] Invalid input. Please enter a valid number.\n";
        }
    }
}

std::string getLineInput(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (std::cin.eof()) return "";
    std::getline(std::cin, line);
    return line;
}

std::string getMultilineInput(const std::string& prompt) {
    std::cout << prompt << "\n";
    std::cout << "(Type your text below. Enter a line with 'END' on its own to finish):\n";
    std::string fullText;
    std::string line;
    while (!std::cin.eof()) {
        std::getline(std::cin, line);
        if (line == "END") {
            break;
        }
        fullText += line + "\n";
    }
    return fullText;
}

// ============================================================================
// Submenu Handlers
// ============================================================================

void handleSearchSubmenu(MiniGit& git) {
    std::cout << "\n--- Search Version History ---\n";
    std::cout << "  1. Search by Version ID\n";
    std::cout << "  2. Search by Filename\n";
    std::cout << "  3. Search by Keyword in Content\n";
    std::cout << "  0. Back to Main Menu\n";

    int searchChoice = getValidatedInteger("Choose search option (0-3): ", 0, 3);
    switch (searchChoice) {
        case 1: {
            int id = getValidatedInteger("Enter Version ID to search: ", 1, 100000);
            git.searchById(id);
            break;
        }
        case 2: {
            std::string filename = getLineInput("Enter filename to search: ");
            git.searchByFilename(filename);
            break;
        }
        case 3: {
            std::string keyword = getLineInput("Enter keyword to search for: ");
            git.searchByKeyword(keyword);
            break;
        }
        case 0:
            std::cout << "Returning to Main Menu.\n";
            break;
    }
}

void handleSortSubmenu(MiniGit& git) {
    std::cout << "\n--- Sort Versions ---\n";
    std::cout << "  1. Sort by Version ID (Ascending)\n";
    std::cout << "  2. Sort by Version ID (Descending)\n";
    std::cout << "  3. Sort by Timestamp (Ascending: Oldest First)\n";
    std::cout << "  4. Sort by Timestamp (Descending: Newest First)\n";
    std::cout << "  0. Back to Main Menu\n";

    int sortChoice = getValidatedInteger("Choose sort option (0-4): ", 0, 4);
    switch (sortChoice) {
        case 1:
            git.sortVersionsById(true);
            break;
        case 2:
            git.sortVersionsById(false);
            break;
        case 3:
            git.sortVersionsByTimestamp(true);
            break;
        case 4:
            git.sortVersionsByTimestamp(false);
            break;
        case 0:
            std::cout << "Returning to Main Menu.\n";
            break;
    }
}

// ============================================================================
// Main Application Loop
// ============================================================================

int main() {
    MiniGit git;
    bool running = true;

    std::cout << "====================================================\n";
    std::cout << "                 MINI GIT (C++)                     \n";
    std::cout << "       Simplified File Version Control System       \n";
    std::cout << "    University Data Structures & Algorithms Project \n";
    std::cout << "====================================================\n";

    while (running) {
        std::cout << "\n==================== MAIN MENU ====================\n";
        std::cout << "  1. Create New File\n";
        std::cout << "  2. Edit Existing File\n";
        std::cout << "  3. View File Content\n";
        std::cout << "  4. Save New Version\n";
        std::cout << "  5. View Version History\n";
        std::cout << "  6. Search Version History\n";
        std::cout << "  7. Compare Two Versions\n";
        std::cout << "  8. Restore Previous Version\n";
        std::cout << "  9. Undo\n";
        std::cout << " 10. Redo\n";
        std::cout << " 11. Sort Versions\n";
        std::cout << " 12. Save Project Data\n";
        std::cout << " 13. Load Project Data\n";
        std::cout << " 14. Exit\n";
        std::cout << "====================================================\n";

        int choice = getValidatedInteger("Enter your choice (1-14): ", 1, 14);

        switch (choice) {
            case 1: { // 1. Create New File
                std::string filename = getLineInput("Enter new filename (e.g., test.txt): ");
                git.createNewFile(filename);
                break;
            }

            case 2: { // 2. Edit Existing File
                std::string filename = getLineInput("Enter filename to edit: ");
                std::cout << "Select edit mode:\n  1. Append content\n  2. Replace content\n";
                int mode = getValidatedInteger("Choose mode (1-2): ", 1, 2);
                std::string content = getMultilineInput("Enter new content:");
                git.editExistingFile(filename, content, (mode == 1));
                break;
            }

            case 3: { // 3. View File Content
                std::string filename = getLineInput("Enter filename to view: ");
                git.viewFileContent(filename);
                break;
            }

            case 4: { // 4. Save New Version
                std::string filename = getLineInput("Enter filename to commit as a new version: ");
                git.saveNewVersion(filename);
                break;
            }

            case 5: { // 5. View Version History
                std::string filename = getLineInput("Enter filename to filter history (or press Enter for all): ");
                git.viewVersionHistory(filename);
                break;
            }

            case 6: { // 6. Search Version History
                handleSearchSubmenu(git);
                break;
            }

            case 7: { // 7. Compare Two Versions
                std::cout << "Enter Version IDs to compare:\n";
                int id1 = getValidatedInteger("First Version ID: ", 1, 100000);
                int id2 = getValidatedInteger("Second Version ID: ", 1, 100000);
                git.compareTwoVersions(id1, id2);
                break;
            }

            case 8: { // 8. Restore Previous Version
                int id = getValidatedInteger("Enter Version ID to restore: ", 1, 100000);
                std::string target = getLineInput("Enter destination filename to overwrite (e.g., main.cpp): ");
                git.restorePreviousVersion(id, target);
                break;
            }

            case 9: { // 9. Undo
                git.undo();
                break;
            }

            case 10: { // 10. Redo
                git.redo();
                break;
            }

            case 11: { // 11. Sort Versions
                handleSortSubmenu(git);
                break;
            }

            case 12: { // 12. Save Project Data
                git.saveProjectData();
                break;
            }

            case 13: { // 13. Load Project Data
                git.loadProjectData();
                break;
            }

            case 14: { // 14. Exit
                std::cout << "\nThank you for using Mini Git. Goodbye!\n";
                running = false;
                break;
            }
        }
    }

    return 0;
}
