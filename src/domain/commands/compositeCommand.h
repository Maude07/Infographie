#pragma once

#include "command.h"
#include <memory>
#include <vector>

class CompositeCommand : public Command {
public:
	CompositeCommand(std::vector<std::unique_ptr<Command>> commands)
		: commands(std::move(commands)) { }

	void undo() override {
		for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
			(*it)->undo();
		}
	}

	void redo() override {
		for (auto & cmd : commands) {
			cmd->redo();
		}
	}

private:
	std::vector<std::unique_ptr<Command>> commands;
};
