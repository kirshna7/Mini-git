#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <string>
#include "FileManager.h"

/**
 * Class: StackNode
 * ----------------
 * Represents a single node inside the custom linked-list-based Stack.
 * Stores a snapshot of the file state (filename, content, and edit description).
 */
class StackNode {
private:
    std::string filename;          // Name of the affected file
    std::string content;           // Snapshot of file content
    std::string description;       // Action description (e.g., "Added Line 2")
    StackNode* next;               // Pointer to the node below on the stack

public:
    StackNode(const std::string& fname, const std::string& fileContent, const std::string& desc);
    ~StackNode();

    // Getters
    std::string getFilename() const;
    std::string getContent() const;
    std::string getDescription() const;
    StackNode* getNext() const;

    // Setter
    void setNext(StackNode* nextNode);
};

/**
 * Class: ActionStack
 * ------------------
 * Custom LIFO (Last-In-First-Out) Stack implemented with singly linked nodes.
 * Does NOT use std::stack, std::vector, or std::list.
 */
class ActionStack {
private:
    StackNode* topNode;            // Pointer to the top of the stack
    int stackSize;                 // Number of items on the stack

public:
    ActionStack();
    ~ActionStack();

    // 1. Push a new state onto the stack (O(1))
    void push(const std::string& fname, const std::string& fileContent, const std::string& desc);

    // 2. Pop the top state from the stack (O(1)) - returns false if stack is empty
    bool pop(std::string& outFilename, std::string& outContent, std::string& outDescription);

    // 3. Inspect top state without removing it (O(1)) - returns false if stack is empty
    bool peek(std::string& outFilename, std::string& outContent, std::string& outDescription) const;

    // 4. Check if the stack is empty (O(1))
    bool isEmpty() const;

    // Get number of elements
    int size() const;

    // Direct pointer to top of stack (used for persistence)
    StackNode* getTop() const;

    // Clear all elements and free allocated memory
    void clear();

    // Display all items currently on the stack from top to bottom
    void displayStack(const std::string& stackName) const;
};

/**
 * Class: UndoRedoManager
 * ----------------------
 * Manages two separate ActionStack instances:
 *   - undoStack: Stores previous states to revert to.
 *   - redoStack: Stores undone states to re-apply.
 *
 * Enforces rule: Any new edit clears the redoStack.
 */
class UndoRedoManager {
private:
    ActionStack undoStack;
    ActionStack redoStack;

public:
    UndoRedoManager();
    ~UndoRedoManager();

    // Record a new edit: pushes old content onto undoStack and CLEARS redoStack
    void recordEdit(const std::string& filename, const std::string& oldContent, const std::string& description);

    // Direct push for reconstruction during load
    void pushUndoRaw(const std::string& filename, const std::string& content, const std::string& description);
    void pushRedoRaw(const std::string& filename, const std::string& content, const std::string& description);

    // Reverts the last action using undoStack and updates disk file via FileManager
    bool undo(FileManager& fileManager);

    // Re-applies the last undone action using redoStack and updates disk file via FileManager
    bool redo(FileManager& fileManager);

    // Accessors for persistence
    const ActionStack& getUndoStack() const;
    const ActionStack& getRedoStack() const;

    // Checkers
    bool canUndo() const;
    bool canRedo() const;

    // Clear both stacks
    void clearAll();

    // Display current states of both stacks (useful for demonstrations & debugging)
    void printStatus() const;
};

#endif // UNDOREDO_H
