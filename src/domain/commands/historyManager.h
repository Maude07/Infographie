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

	Command * lastUndo() const { return undoStack.empty() ? nullptr : undoStack.back().get(); }

private:
	std::vector<std::unique_ptr<Command>> undoStack;
	std::vector<std::unique_ptr<Command>> redoStack;
};
