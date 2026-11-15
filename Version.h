#ifndef VERSION_H
#define VERSION_H

#include <string>

/**
 * Class: Version
 * --------------
 * Represents a single node in the singly linked list of version history.
 * Contains metadata and the snapshot of the file at the time of saving.
 */
class Version {
private:
    int versionId;             // Unique sequential ID (1, 2, 3...)
    std::string filename;      // Name of the tracked file
    std::string content;       // Snapshot of the file content
    std::string timestamp;     // Date and time the version was created (YYYY-MM-DD HH:MM:SS)
    Version* next;             // Pointer to the next version node

public:
    // Constructor
    Version(int id, const std::string& fname, const std::string& fileContent, const std::string& timeStr);

    // Destructor
    ~Version();

    // Getters
    int getVersionId() const;
    std::string getFilename() const;
    std::string getContent() const;
    std::string getTimestamp() const;
    Version* getNext() const;

    // Setters
    void setNext(Version* nextNode);
    void setVersionId(int id);
    void setFilename(const std::string& fname);
    void setContent(const std::string& fileContent);
    void setTimestamp(const std::string& timeStr);

    // Swaps data payload with another node (used by sorting algorithms to preserve pointer safety)
    void swapData(Version* other);

    // Display formatted details of this single version
    void printDetails() const;
};

#endif // VERSION_H
