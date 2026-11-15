#include "Version.h"
#include <iostream>
#include <utility>

Version::Version(int id, const std::string& fname, const std::string& fileContent, const std::string& timeStr)
    : versionId(id), filename(fname), content(fileContent), timestamp(timeStr), next(nullptr) {}

Version::~Version() {}

int Version::getVersionId() const {
    return versionId;
}

std::string Version::getFilename() const {
    return filename;
}

std::string Version::getContent() const {
    return content;
}

std::string Version::getTimestamp() const {
    return timestamp;
}

Version* Version::getNext() const {
    return next;
}

void Version::setNext(Version* nextNode) {
    next = nextNode;
}

void Version::setVersionId(int id) {
    versionId = id;
}

void Version::setFilename(const std::string& fname) {
    filename = fname;
}

void Version::setContent(const std::string& fileContent) {
    content = fileContent;
}

void Version::setTimestamp(const std::string& timeStr) {
    timestamp = timeStr;
}

// Swaps content values between two nodes without mutating next pointers
void Version::swapData(Version* other) {
    if (other == nullptr) return;
    std::swap(versionId, other->versionId);
    std::swap(filename, other->filename);
    std::swap(content, other->content);
    std::swap(timestamp, other->timestamp);
}

void Version::printDetails() const {
    std::cout << "--------------------------------------------------\n";
    std::cout << " Version ID : " << versionId << "\n";
    std::cout << " File       : " << filename << "\n";
    std::cout << " Timestamp  : " << timestamp << "\n";
    std::cout << " Content    :\n" << content;
    if (!content.empty() && content.back() != '\n') {
        std::cout << "\n";
    }
    std::cout << "--------------------------------------------------\n";
}
