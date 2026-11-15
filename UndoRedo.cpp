#include "UndoRedo.h"
#include <iostream>

// ============================================================================
// StackNode Implementation
// ============================================================================

StackNode::StackNode(const std::string& fname, const std::string& fileContent, const std::string& desc)
    : filename(fname), content(fileContent), description(desc), next(nullptr) {}

StackNode::~StackNode() {}

std::string StackNode::getFilename() const {
    return filename;
}

std::string StackNode::getContent() const {
    return content;
}

std::string StackNode::getDescription() const {
    return description;
}

StackNode* StackNode::getNext() const {
    return next;
}

void StackNode::setNext(StackNode* nextNode) {
    next = nextNode;
}

// ============================================================================
// ActionStack Implementation (Custom LIFO Linked List Stack)
// ============================================================================

ActionStack::ActionStack() : topNode(nullptr), stackSize(0) {}

ActionStack::~ActionStack() {
    clear();
}

void ActionStack::push(const std::string& fname, const std::string& fileContent, const std::string& desc) {
    StackNode* newNode = new StackNode(fname, fileContent, desc);
    newNode->setNext(topNode);
    topNode = newNode;
    stackSize++;
}

bool ActionStack::pop(std::string& outFilename, std::string& outContent, std::string& outDescription) {
    if (isEmpty()) {
        return false;
    }

    StackNode* nodeToRemove = topNode;
    outFilename = nodeToRemove->getFilename();
    outContent = nodeToRemove->getContent();
    outDescription = nodeToRemove->getDescription();

    topNode = topNode->getNext();
    delete nodeToRemove;
    stackSize--;
    return true;
}

bool ActionStack::peek(std::string& outFilename, std::string& outContent, std::string& outDescription) const {
    if (isEmpty()) {
        return false;
    }
    outFilename = topNode->getFilename();
    outContent = topNode->getContent();
    outDescription = topNode->getDescription();
    return true;
}

bool ActionStack::isEmpty() const {
    return topNode == nullptr;
}

int ActionStack::size() const {
    return stackSize;
}

StackNode* ActionStack::getTop() const {
    return topNode;
}

void ActionStack::clear() {
    while (topNode != nullptr) {
        StackNode* temp = topNode;
        topNode = topNode->getNext();
        delete temp;
    }
    stackSize = 0;
}

void ActionStack::displayStack(const std::string& stackName) const {
    std::cout << "--- " << stackName << " (Size: " << stackSize << ") ---\n";
    if (isEmpty()) {
        std::cout << "  [Empty]\n";
        return;
    }

    StackNode* current = topNode;
    int index = 1;
    while (current != nullptr) {
        std::cout << "  " << index++ << ". [" << current->getDescription() << "] File: "
                  << current->getFilename() << " (Length: " << current->getContent().length() << " chars)\n";
        current = current->getNext();
    }
}

// ============================================================================
// UndoRedoManager Implementation
// ============================================================================

UndoRedoManager::UndoRedoManager() {}

UndoRedoManager::~UndoRedoManager() {
    clearAll();
}

void UndoRedoManager::recordEdit(const std::string& filename, const std::string& oldContent, const std::string& description) {
    undoStack.push(filename, oldContent, description);
    redoStack.clear();
    std::cout << "[UndoRedo] Recorded action: \"" << description << "\" on Undo Stack.\n";
    std::cout << "[UndoRedo] Redo Stack cleared.\n";
}

void UndoRedoManager::pushUndoRaw(const std::string& filename, const std::string& content, const std::string& description) {
    undoStack.push(filename, content, description);
}

void UndoRedoManager::pushRedoRaw(const std::string& filename, const std::string& content, const std::string& description) {
    redoStack.push(filename, content, description);
}

bool UndoRedoManager::undo(FileManager& fileManager) {
    if (!canUndo()) {
        std::cout << "[Notice] Nothing to undo. Undo Stack is empty.\n";
        return false;
    }

    std::string filename, previousContent, description;
    undoStack.pop(filename, previousContent, description);

    std::string currentContent;
    fileManager.readFile(filename, currentContent);

    redoStack.push(filename, currentContent, "Undo: " + description);

    bool restored = fileManager.saveFile(filename, previousContent);
    if (restored) {
        std::cout << "[Success] UNDO executed: Restored state for '" << filename << "' (" << description << ").\n";
    }
    return restored;
}

bool UndoRedoManager::redo(FileManager& fileManager) {
    if (!canRedo()) {
        std::cout << "[Notice] Nothing to redo. Redo Stack is empty.\n";
        return false;
    }

    std::string filename, nextContent, description;
    redoStack.pop(filename, nextContent, description);

    std::string currentContent;
    fileManager.readFile(filename, currentContent);

    undoStack.push(filename, currentContent, "Redo: " + description);

    bool reapplied = fileManager.saveFile(filename, nextContent);
    if (reapplied) {
        std::cout << "[Success] REDO executed: Re-applied state for '" << filename << "' (" << description << ").\n";
    }
    return reapplied;
}

const ActionStack& UndoRedoManager::getUndoStack() const {
    return undoStack;
}

const ActionStack& UndoRedoManager::getRedoStack() const {
    return redoStack;
}

bool UndoRedoManager::canUndo() const {
    return !undoStack.isEmpty();
}

bool UndoRedoManager::canRedo() const {
    return !redoStack.isEmpty();
}

void UndoRedoManager::clearAll() {
    undoStack.clear();
    redoStack.clear();
}

void UndoRedoManager::printStatus() const {
    std::cout << "\n================ STACK STATUS ================\n";
    undoStack.displayStack("UNDO STACK (Past States)");
    redoStack.displayStack("REDO STACK (Undone States)");
    std::cout << "==============================================\n\n";
}
