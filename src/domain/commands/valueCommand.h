#pragma once

#include "command.h"

template <typename T>
class ValueCommand : public Command {
public:
	ValueCommand(T & target, T oldValue, T newValue)
		: target(target)
		, oldValue(oldValue)
		, newValue(newValue) { }

	void undo() override { target = oldValue; }
	void redo() override { target = newValue; }

private:
	T & target;
	T oldValue;
	T newValue;
};
