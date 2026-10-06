
#include "historyManager.h"

using namespace std;

void HistoryManager::push(unique_ptr<Command> command) {
	undoStack.push_back(move(command));
	redoStack.clear();
}

void HistoryManager::execute(unique_ptr<Command> command) {
	command->redo();
	undoStack.push_back(move(command));
	redoStack.clear();
}

void HistoryManager::undo() {
	if (undoStack.empty()) return;
	auto command = move(undoStack.back());
	undoStack.pop_back();
	command->undo();
	redoStack.push_back(move(command));
}

void HistoryManager::redo() {
	if (redoStack.empty()) return;
	auto command = move(redoStack.back());
	redoStack.pop_back();
	command->redo();
	undoStack.push_back(move(command));
}

void HistoryManager::clear() {
	undoStack.clear();
	redoStack.clear();
}
