#include "VersionHistory.h"
#include <iostream>
#include <ctime>

std::string VersionHistory::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

VersionHistory::VersionHistory()
    : head(nullptr), tail(nullptr), totalVersions(0), nextVersionId(1) {}

VersionHistory::~VersionHistory() {
    clear();
}

void VersionHistory::clear() {
    Version* current = head;
    while (current != nullptr) {
        Version* nextNode = current->getNext();
        delete current;
        current = nextNode;
    }
    head = nullptr;
    tail = nullptr;
    totalVersions = 0;
    nextVersionId = 1;
}

Version* VersionHistory::addVersion(const std::string& filename, const std::string& content) {
    return addVersionWithTimestamp(filename, content, getCurrentTimestamp());
}

Version* VersionHistory::addVersionWithTimestamp(const std::string& filename, const std::string& content, const std::string& timestamp) {
    return addVersionExplicit(nextVersionId++, filename, content, timestamp);
}

Version* VersionHistory::addVersionExplicit(int id, const std::string& filename, const std::string& content, const std::string& timestamp) {
    Version* newVersion = new Version(id, filename, content, timestamp);

    if (head == nullptr) {
        head = newVersion;
        tail = newVersion;
    } else {
        tail->setNext(newVersion);
        tail = newVersion;
    }

    totalVersions++;
    if (id >= nextVersionId) {
        nextVersionId = id + 1;
    }

    std::cout << "[VersionHistory] Version " << newVersion->getVersionId()
              << " created for '" << filename << "' at " << timestamp << "\n";
    return newVersion;
}

void VersionHistory::displayVersionsOfFile(const std::string& filename) const {
    if (head == nullptr) {
        std::cout << "[Notice] Version history is empty.\n";
        return;
    }

    std::cout << "\n==================================================\n";
    std::cout << "        Version History for: " << filename << "\n";
    std::cout << "==================================================\n";

    Version* current = head;
    int matchCount = 0;

    while (current != nullptr) {
        if (current->getFilename() == filename) {
            current->printDetails();
            matchCount++;
        }
        current = current->getNext();
    }

    if (matchCount == 0) {
        std::cout << "No recorded versions found for '" << filename << "'.\n";
    } else {
        std::cout << "Total versions for '" << filename << "': " << matchCount << "\n";
    }
    std::cout << "==================================================\n\n";
}

void VersionHistory::displayAllVersions() const {
    if (head == nullptr) {
        std::cout << "[Notice] Version history is empty. No commits recorded yet.\n";
        return;
    }

    std::cout << "\n==================================================\n";
    std::cout << "             Complete Version History             \n";
    std::cout << "==================================================\n";

    Version* current = head;
    while (current != nullptr) {
        current->printDetails();
        current = current->getNext();
    }
    std::cout << "Total active versions in repository: " << totalVersions << "\n";
    std::cout << "==================================================\n\n";
}

Version* VersionHistory::searchVersionById(int versionId) const {
    Version* current = head;
    while (current != nullptr) {
        if (current->getVersionId() == versionId) {
            return current;
        }
        current = current->getNext();
    }
    return nullptr;
}

Version* VersionHistory::findVersionById(int versionId) const {
    return searchVersionById(versionId);
}

Version* VersionHistory::getLatestVersion() const {
    return tail;
}

void VersionHistory::displayLatestVersion() const {
    if (tail == nullptr) {
        std::cout << "[Notice] No versions exist yet.\n";
        return;
    }

    std::cout << "\n>>> LATEST VERSION <<<\n";
    tail->printDetails();
}

bool VersionHistory::deleteVersion(int versionId) {
    if (head == nullptr) {
        std::cerr << "[Error] Cannot delete: Version history is empty.\n";
        return false;
    }

    if (head->getVersionId() == versionId) {
        Version* nodeToDelete = head;
        head = head->getNext();

        if (head == nullptr) {
            tail = nullptr;
        }

        delete nodeToDelete;
        totalVersions--;
        std::cout << "[Success] Version " << versionId << " (HEAD) deleted.\n";
        return true;
    }

    Version* prev = head;
    Version* current = head->getNext();

    while (current != nullptr && current->getVersionId() != versionId) {
        prev = current;
        current = current->getNext();
    }

    if (current == nullptr) {
        std::cerr << "[Error] Version ID " << versionId << " not found in history.\n";
        return false;
    }

    prev->setNext(current->getNext());

    if (current == tail) {
        tail = prev;
    }

    delete current;
    totalVersions--;
    std::cout << "[Success] Version " << versionId << " deleted from history.\n";
    return true;
}

bool VersionHistory::restoreVersion(int versionId, const std::string& targetPath, FileManager& fileManager) {
    Version* versionToRestore = searchVersionById(versionId);
    if (versionToRestore == nullptr) {
        std::cerr << "[Error] Cannot restore: Version " << versionId << " does not exist.\n";
        return false;
    }

    bool fileSaved = fileManager.saveFile(targetPath, versionToRestore->getContent());
    if (!fileSaved) {
        std::cerr << "[Error] Failed to write restored content to '" << targetPath << "'.\n";
        return false;
    }

    std::cout << "[Info] Restored content from Version " << versionId << " to '" << targetPath << "'.\n";
    addVersion(targetPath, versionToRestore->getContent());
    return true;
}

int VersionHistory::getTotalVersions() const {
    return totalVersions;
}

int VersionHistory::countVersionsTraversal() const {
    int count = 0;
    Version* current = head;
    while (current != nullptr) {
        count++;
        current = current->getNext();
    }
    return count;
}

Version* VersionHistory::getHead() const {
    return head;
}

Version* VersionHistory::getTail() const {
    return tail;
}

bool VersionHistory::isEmpty() const {
    return head == nullptr;
}
