# Mini Git (C++ Version Control System)

A simplified, educational file version control system built from scratch in modern C++ for a university Data Structures and Algorithms project.

---

## 📌 Project Architecture & Data Structures

- **Version History:** Custom-built **Singly Linked List** maintaining chronological commit versions (`head` to `tail`), supporting $O(1)$ tail insertion, $O(N)$ linear search, and $O(N)$ node deletion.
- **Undo & Redo:** Two separate manually implemented **LIFO Linked-List Stacks** (`undoStack` and `redoStack`) supporting $O(1)$ `push()`, `pop()`, `peek()`, and `isEmpty()`. Any new edit automatically clears the redo stack.
- **File Management:** Standard C++ `std::ifstream`, `std::ofstream`, and `std::fstream` handle safe file creation, line reading, append/replace editing, and permanent flushing to disk.
- **File Comparison (`diff`):** Custom **Longest Common Subsequence (LCS)** line diff algorithm in pure C++ displaying added lines (`+`), removed lines (`-`), and unchanged lines (` `) with statistical summaries.
- **Searching & Sorting:**
  - **Linear Search:** Search by Version ID, by Filename, and by Substring Keyword in saved file contents.
  - **In-Place Bubble Sort on Linked List:** Sorts by Version ID or Timestamp (Ascending / Descending) with 100% pointer integrity and zero lost versions.
- **Permanent Project Storage:**
  - `storage/metadata.txt`: Total versions count and repository state.
  - `storage/version_history.txt`: Serialized index to reconstruct the singly linked list.
  - `storage/versions/`: Isolated directory for version snapshots.
  - `storage/undo_redo_state.txt`: Serialized state to reconstruct undo/redo stacks.
  - Safe error recovery on corrupted or missing records.
- **Object-Oriented Design:** Standard C++ classes with encapsulation (no `struct`s or external libraries).
- **Cross-Platform:** Works natively on macOS, Linux, and Windows.

---

## 📂 Project Structure

```text
MiniGit/
├── main.cpp                 # 14-Option Interactive Console Menu
├── MiniGit.h / .cpp         # Central Controller coordinating all modules
├── FileManager.h / .cpp     # File I/O using ifstream, ofstream, fstream
├── Version.h / .cpp         # Version node class with payload swapping
├── VersionHistory.h / .cpp  # Singly Linked List managing version timeline
├── UndoRedo.h / .cpp        # Custom Linked-List Stacks for Undo and Redo
├── Compare.h / .cpp         # Longest Common Subsequence (LCS) line diff
├── Search.h / .cpp          # Linear Search & In-Place Bubble Sort
├── storage/                 # Permanent Repository Storage
│   ├── metadata.txt         # Repository metadata
│   ├── version_history.txt  # Serialized linked list index
│   ├── undo_redo_state.txt  # Serialized undo/redo stacks
│   └── versions/            # Isolated snapshot backups
└── README.md                # Project documentation & build guide
```

---

## 🛠️ How to Compile and Run

### On macOS / Linux (Clang++ or GCC):
```bash
clang++ -std=c++17 -Wall -Wextra main.cpp MiniGit.cpp FileManager.cpp Version.cpp VersionHistory.cpp UndoRedo.cpp Compare.cpp Search.cpp -o minigit
./minigit
```

### On Windows (MinGW / GCC):
```cmd
g++ -std=c++17 -Wall -Wextra main.cpp MiniGit.cpp FileManager.cpp Version.cpp VersionHistory.cpp UndoRedo.cpp Compare.cpp Search.cpp -o minigit.exe
minigit.exe
```

### On Windows (MSVC Developer Command Prompt):
```cmd
cl /std:c++17 /EHsc *.cpp /Fe:minigit.exe
minigit.exe
```

---

## 📋 Main Menu Operations

1. **Create New File:** Safely creates a new file with overwrite confirmation.
2. **Edit Existing File:** Appends or replaces file text; records prior state on Undo stack.
3. **View File Content:** Displays formatted file text with line numbers.
4. **Save New Version:** Commits file state to Version History linked list.
5. **View Version History:** Traverses chronological version history.
6. **Search Version History:** Linear search by ID, by filename, or by content keyword.
7. **Compare Two Versions:** Line-by-line diff comparing any two versions.
8. **Restore Previous Version:** Restores older file version while preserving history immutability.
9. **Undo:** Reverts file to previous state via Undo Stack.
10. **Redo:** Re-applies undone state via Redo Stack.
11. **Sort Versions:** In-place Bubble Sort by ID or Timestamp (Ascending / Descending).
12. **Save Project Data:** Serializes repository and stacks to disk.
13. **Load Project Data:** Reconstructs linked list and stacks from disk.
14. **Exit:** Gracefully terminates the application.
