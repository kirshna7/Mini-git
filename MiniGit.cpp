#include "MiniGit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// Helper: Sanitize path string for safe snapshot filename in storage/versions/
static std::string sanitizeForSnapshot(const std::string& path) {
    std::string safe = path;
    for (char& ch : safe) {
        if (ch == '/' || ch == '\\') {
            ch = '_';
        }
    }
    return safe;
}

MiniGit::MiniGit() {
    fileManager.createDirectory(storageDir);
    fileManager.createDirectory(versionsDir);
    autoLoadStartup();
}

MiniGit::~MiniGit() {}

// ============================================================================
// Auto-Load on Startup
// ============================================================================
void MiniGit::autoLoadStartup() {
    if (fileManager.fileExists(historyFile)) {
        std::cout << "[Storage] Found existing repository data in 'storage/'. Auto-loading...\n";
        loadProjectData();
    } else {
        std::cout << "[Storage] Initialized new repository storage in 'storage/'.\n";
    }
}

// 1. Create New File
bool MiniGit::createNewFile(const std::string& filename) {
    return fileManager.createFile(filename);
}

// 2. Edit Existing File
bool MiniGit::editExistingFile(const std::string& filename, const std::string& newContent, bool appendMode) {
    if (!fileManager.fileExists(filename)) {
        std::cerr << "[Error] Cannot edit: File '" << filename << "' does not exist.\n";
        return false;
    }

    std::string currentContent;
    fileManager.readFile(filename, currentContent);

    std::string desc = appendMode ? "Appended text" : "Replaced content";
    undoRedo.recordEdit(filename, currentContent, desc);

    return fileManager.editFile(filename, newContent, appendMode);
}

// 3. View File Content
void MiniGit::viewFileContent(const std::string& filename) const {
    fileManager.readAndDisplayFile(filename);
}

// 4. Save New Version
bool MiniGit::saveNewVersion(const std::string& filename) {
    if (!fileManager.fileExists(filename)) {
        std::cerr << "[Error] Cannot commit version: File '" << filename << "' does not exist.\n";
        return false;
    }

    std::string content;
    if (!fileManager.readFile(filename, content)) {
        std::cerr << "[Error] Failed to read content from '" << filename << "'.\n";
        return false;
    }

    Version* v = history.addVersion(filename, content);
    if (v != nullptr) {
        // Automatically save an isolated snapshot file in storage/versions/
        std::string snapshotPath = versionsDir + "/v" + std::to_string(v->getVersionId()) + "_" + sanitizeForSnapshot(filename) + ".txt";
        fileManager.saveFile(snapshotPath, content);

        std::cout << "[Success] Committed Version " << v->getVersionId()
                  << " of '" << filename << "' to Version History.\n";
        std::cout << "[Storage] Snapshot stored in '" << snapshotPath << "'.\n";
        return true;
    }
    return false;
}

// 5. View Version History
void MiniGit::viewVersionHistory(const std::string& filename) const {
    if (filename.empty()) {
        history.displayAllVersions();
    } else {
        history.displayVersionsOfFile(filename);
    }
}

// 6. Search Operations
void MiniGit::searchById(int id) const {
    Version* v = searchEngine.searchById(history, id);
    if (v != nullptr) {
        std::cout << "[Match Found] Version details:\n";
        v->printDetails();
    } else {
        std::cout << "[Notice] No version found with ID " << id << ".\n";
    }
}

void MiniGit::searchByFilename(const std::string& filename) const {
    std::vector<Version*> matches = searchEngine.searchByFilename(history, filename);
    if (matches.empty()) {
        std::cout << "[Notice] No versions found for file '" << filename << "'.\n";
        return;
    }

    std::cout << "\nFound " << matches.size() << " version(s) matching '" << filename << "':\n";
    for (Version* v : matches) {
        v->printDetails();
    }
}

void MiniGit::searchByKeyword(const std::string& keyword) const {
    std::vector<Version*> matches = searchEngine.searchByKeyword(history, keyword);
    if (matches.empty()) {
        std::cout << "[Notice] No versions found containing keyword '" << keyword << "'.\n";
        return;
    }

    std::cout << "\nFound " << matches.size() << " version(s) containing keyword '" << keyword << "':\n";
    for (Version* v : matches) {
        v->printDetails();
    }
}

// 7. Compare Two Versions
void MiniGit::compareTwoVersions(int id1, int id2) const {
    if (history.getTotalVersions() < 2) {
        std::cout << "[Notice] At least 2 versions are required to run comparison.\n";
        return;
    }
    comparator.compareFileVersions(history, id1, id2);
}

// 8. Restore Previous Version
bool MiniGit::restorePreviousVersion(int id, const std::string& targetFilename) {
    Version* v = history.searchVersionById(id);
    if (v == nullptr) {
        std::cerr << "[Error] Cannot restore: Version " << id << " does not exist.\n";
        return false;
    }

    if (fileManager.fileExists(targetFilename)) {
        std::string currentDisk;
        fileManager.readFile(targetFilename, currentDisk);
        undoRedo.recordEdit(targetFilename, currentDisk, "Before restore of Version " + std::to_string(id));
    }

    return history.restoreVersion(id, targetFilename, fileManager);
}

// 9. Undo
void MiniGit::undo() {
    undoRedo.undo(fileManager);
}

// 10. Redo
void MiniGit::redo() {
    undoRedo.redo(fileManager);
}

// 11. Sorting
void MiniGit::sortVersionsById(bool ascending) {
    searchEngine.sortById(history, ascending);
    std::cout << "[Success] Version History sorted by ID ("
              << (ascending ? "Ascending" : "Descending") << ").\n";
    history.displayAllVersions();
}

void MiniGit::sortVersionsByTimestamp(bool ascending) {
    searchEngine.sortByTimestamp(history, ascending);
    std::cout << "[Success] Version History sorted by Timestamp ("
              << (ascending ? "Oldest First" : "Newest First") << ").\n";
    history.displayAllVersions();
}

// ============================================================================
// 12. Permanent Storage: Save Project Data (With Atomic Protection)
// ============================================================================
bool MiniGit::saveProjectData() {
    fileManager.createDirectory(storageDir);
    fileManager.createDirectory(versionsDir);

    // Part A: Save Version History Linked List & Snapshots using Atomic Temp File
    std::string historyTmp = historyFile + ".tmp";
    std::ofstream historyOut(historyTmp, std::ios::out | std::ios::trunc);
    if (!historyOut.is_open()) {
        std::cerr << "[Error] Failed to open '" << historyTmp << "' for writing.\n";
        return false;
    }

    int versionCount = 0;
    Version* current = history.getHead();
    while (current != nullptr) {
        historyOut << "===VERSION_RECORD===\n";
        historyOut << "ID:" << current->getVersionId() << "\n";
        historyOut << "FILE:" << current->getFilename() << "\n";
        historyOut << "TIMESTAMP:" << current->getTimestamp() << "\n";

        // Save isolated snapshot file
        std::string snapshotFile = versionsDir + "/v" + std::to_string(current->getVersionId())
                                 + "_" + sanitizeForSnapshot(current->getFilename()) + ".txt";
        fileManager.saveFile(snapshotFile, current->getContent());
        historyOut << "SNAPSHOT:" << snapshotFile << "\n";

        historyOut << "---CONTENT_START---\n";
        historyOut << current->getContent();
        if (!current->getContent().empty() && current->getContent().back() != '\n') {
            historyOut << "\n";
        }
        historyOut << "---CONTENT_END---\n";
        historyOut << "===END_RECORD===\n";

        versionCount++;
        current = current->getNext();
    }

    historyOut.flush();
    historyOut.close();

    // Atomic replacement: only replace original file when write succeeded completely
    std::remove(historyFile.c_str());
    std::rename(historyTmp.c_str(), historyFile.c_str());

    // Part B: Save Metadata Atomically
    std::string metaTmp = metadataFile + ".tmp";
    std::ofstream metaOut(metaTmp, std::ios::out | std::ios::trunc);
    if (metaOut.is_open()) {
        metaOut << "TOTAL_VERSIONS:" << versionCount << "\n";
        metaOut.flush();
        metaOut.close();
        std::remove(metadataFile.c_str());
        std::rename(metaTmp.c_str(), metadataFile.c_str());
    }

    // Part C: Save Undo & Redo Stacks Atomically
    std::string urTmp = undoRedoFile + ".tmp";
    std::ofstream urOut(urTmp, std::ios::out | std::ios::trunc);
    if (urOut.is_open()) {
        // Save Undo Stack
        urOut << "[UNDO_STACK]\n";
        StackNode* uNode = undoRedo.getUndoStack().getTop();
        while (uNode != nullptr) {
            urOut << "---ITEM---\n";
            urOut << "FILE:" << uNode->getFilename() << "\n";
            urOut << "DESC:" << uNode->getDescription() << "\n";
            urOut << "DATA_START\n";
            urOut << uNode->getContent();
            if (!uNode->getContent().empty() && uNode->getContent().back() != '\n') {
                urOut << "\n";
            }
            urOut << "DATA_END\n";
            uNode = uNode->getNext();
        }
        urOut << "[END_UNDO_STACK]\n";

        // Save Redo Stack
        urOut << "[REDO_STACK]\n";
        StackNode* rNode = undoRedo.getRedoStack().getTop();
        while (rNode != nullptr) {
            urOut << "---ITEM---\n";
            urOut << "FILE:" << rNode->getFilename() << "\n";
            urOut << "DESC:" << rNode->getDescription() << "\n";
            urOut << "DATA_START\n";
            urOut << rNode->getContent();
            if (!rNode->getContent().empty() && rNode->getContent().back() != '\n') {
                urOut << "\n";
            }
            urOut << "DATA_END\n";
            rNode = rNode->getNext();
        }
        urOut << "[END_REDO_STACK]\n";

        urOut.flush();
        urOut.close();
        std::remove(undoRedoFile.c_str());
        std::rename(urTmp.c_str(), undoRedoFile.c_str());
    }

    std::cout << "[Success] Permanent storage saved successfully:\n";
    std::cout << "  - Version History: " << versionCount << " versions recorded in '" << historyFile << "'\n";
    std::cout << "  - Snapshots stored in '" << versionsDir << "/'\n";
    std::cout << "  - Undo/Redo states saved in '" << undoRedoFile << "'\n";
    return true;
}

// ============================================================================
// 13. Permanent Storage: Load Project Data (With Safe Corruption Handling)
// ============================================================================
bool MiniGit::loadProjectData() {
    if (!fileManager.fileExists(historyFile)) {
        std::cerr << "[Notice] No existing storage found at '" << historyFile << "'.\n";
        return false;
    }

    std::ifstream historyIn(historyFile, std::ios::in);
    if (!historyIn.is_open()) {
        std::cerr << "[Error] Failed to open '" << historyFile << "' for reading.\n";
        return false;
    }

    // Clear active memory before reconstruction
    history.clear();
    undoRedo.clearAll();

    std::string line;
    int loadedVersions = 0;

    // Parse Version History with safe corruption handling
    while (std::getline(historyIn, line)) {
        if (line == "===VERSION_RECORD===") {
            int id = -1;
            std::string filename, timestamp, content;

            try {
                // Read ID
                if (std::getline(historyIn, line) && line.rfind("ID:", 0) == 0) {
                    id = std::stoi(line.substr(3));
                }
                // Read FILE
                if (std::getline(historyIn, line) && line.rfind("FILE:", 0) == 0) {
                    filename = line.substr(5);
                }
                // Read TIMESTAMP
                if (std::getline(historyIn, line) && line.rfind("TIMESTAMP:", 0) == 0) {
                    timestamp = line.substr(10);
                }
                // Skip SNAPSHOT line if present
                std::streampos pos = historyIn.tellg();
                if (std::getline(historyIn, line)) {
                    if (line.rfind("SNAPSHOT:", 0) != 0) {
                        historyIn.seekg(pos); // Rewind if not snapshot line
                    }
                }
                // Read CONTENT block
                if (std::getline(historyIn, line) && line == "---CONTENT_START---") {
                    while (std::getline(historyIn, line) && line != "---CONTENT_END---") {
                        content += line + "\n";
                    }
                }

                // Verify valid record structure
                if (id != -1 && !filename.empty()) {
                    history.addVersionExplicit(id, filename, content, timestamp);
                    loadedVersions++;
                } else {
                    std::cerr << "[Warning] Skipping malformed version record.\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "[Warning] Corrupted data record encountered: " << e.what() << ". Skipping.\n";
            }
        }
    }
    historyIn.close();

    // Reconstruct Undo & Redo Stacks (if present)
    if (fileManager.fileExists(undoRedoFile)) {
        std::ifstream urIn(undoRedoFile, std::ios::in);
        if (urIn.is_open()) {
            std::string section;
            std::vector<std::pair<std::pair<std::string, std::string>, std::string>> undoItems;
            std::vector<std::pair<std::pair<std::string, std::string>, std::string>> redoItems;

            while (std::getline(urIn, line)) {
                if (line == "[UNDO_STACK]") section = "UNDO";
                else if (line == "[REDO_STACK]") section = "REDO";
                else if (line == "[END_UNDO_STACK]" || line == "[END_REDO_STACK]") section = "";

                if (line == "---ITEM---") {
                    std::string uFile, uDesc, uData;
                    if (std::getline(urIn, line) && line.rfind("FILE:", 0) == 0) uFile = line.substr(5);
                    if (std::getline(urIn, line) && line.rfind("DESC:", 0) == 0) uDesc = line.substr(5);
                    if (std::getline(urIn, line) && line == "DATA_START") {
                        while (std::getline(urIn, line) && line != "DATA_END") {
                            uData += line + "\n";
                        }
                    }
                    if (section == "UNDO") {
                        undoItems.push_back({{uFile, uData}, uDesc});
                    } else if (section == "REDO") {
                        redoItems.push_back({{uFile, uData}, uDesc});
                    }
                }
            }
            urIn.close();

            // Reconstruct in reverse order so original top remains on top
            for (auto it = undoItems.rbegin(); it != undoItems.rend(); ++it) {
                undoRedo.pushUndoRaw(it->first.first, it->first.second, it->second);
            }
            for (auto it = redoItems.rbegin(); it != redoItems.rend(); ++it) {
                undoRedo.pushRedoRaw(it->first.first, it->first.second, it->second);
            }
        }
    }

    std::cout << "[Success] Loaded " << loadedVersions << " versions and restored repository state.\n";
    return true;
}

FileManager& MiniGit::getFileManager() {
    return fileManager;
}

VersionHistory& MiniGit::getHistory() {
    return history;
}

UndoRedoManager& MiniGit::getUndoRedo() {
    return undoRedo;
}
