#pragma once
// IFT3100A25_Interface/commands/historyManager.h
#pragma once

#include "command.h"
#include <memory>
#include <vector>

class HistoryManager {
public:
	void push(std::unique_ptr<Command> command);
	void execute(std::unique_ptr<Command> command);
	void undo();
	void redo();
	void clear();

	bool canUndo() const { return !undoStack.empty(); }
	bool canRedo() const { return !redoStack.empty(); }

private:
	std::vector<std::unique_ptr<Command>> undoStack;
	std::vector<std::unique_ptr<Command>> redoStack;
};
