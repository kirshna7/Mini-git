#ifndef MINIGIT_H
#define MINIGIT_H

#include <string>
#include <vector>
#include "FileManager.h"
#include "VersionHistory.h"
#include "UndoRedo.h"
#include "Compare.h"
#include "Search.h"

/**
 * Class: MiniGit
 * --------------
 * The central controller orchestrating all Mini Git subsystems.
 *
 * Permanent Storage System:
 * - storage/metadata.txt          : Repository metadata and counters
 * - storage/version_history.txt   : Linked list serialized index
 * - storage/versions/             : Isolated version snapshot files
 * - storage/undo_redo_state.txt   : Serialized undo & redo stacks
 */
class MiniGit {
private:
    FileManager fileManager;
    VersionHistory history;
    UndoRedoManager undoRedo;
    Compare comparator;
    Search searchEngine;

    const std::string storageDir = "storage";
    const std::string versionsDir = "storage/versions";
    const std::string historyFile = "storage/version_history.txt";
    const std::string undoRedoFile = "storage/undo_redo_state.txt";
    const std::string metadataFile = "storage/metadata.txt";

    // Auto-load state on startup
    void autoLoadStartup();

public:
    MiniGit();
    ~MiniGit();

    // 1. Create a new text file
    bool createNewFile(const std::string& filename);

    // 2. Edit existing file (records prior state on Undo stack & updates disk)
    bool editExistingFile(const std::string& filename, const std::string& newContent, bool appendMode);

    // 3. View content of a file on disk
    void viewFileContent(const std::string& filename) const;

    // 4. Save a new version of a file into Version History
    bool saveNewVersion(const std::string& filename);

    // 5. View Version History (all or filtered by file)
    void viewVersionHistory(const std::string& filename = "") const;

    // 6. Searching operations
    void searchById(int id) const;
    void searchByFilename(const std::string& filename) const;
    void searchByKeyword(const std::string& keyword) const;

    // 7. Compare two versions
    void compareTwoVersions(int id1, int id2) const;

    // 8. Restore an older version back into a file
    bool restorePreviousVersion(int id, const std::string& targetFilename);

    // 9. Undo last file modification
    void undo();

    // 10. Redo last undone modification
    void redo();

    // 11. Sorting operations
    void sortVersionsById(bool ascending);
    void sortVersionsByTimestamp(bool ascending);

    // 12. Save project data permanently
    bool saveProjectData();

    // 13. Load project data from disk
    bool loadProjectData();

    // Accessors
    FileManager& getFileManager();
    VersionHistory& getHistory();
    UndoRedoManager& getUndoRedo();
};

#endif // MINIGIT_H
