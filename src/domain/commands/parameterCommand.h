#pragma once

#include "command.h"
#include "ofMain.h"

template <typename T>
class ParameterCommand : public Command {
public:
	ParameterCommand(ofParameter<T> & parameter, T oldValue, T newValue)
		: parameter(parameter)
		, oldValue(oldValue)
		, newValue(newValue) { }

	void undo() override { parameter = oldValue; }
	void redo() override { parameter = newValue; }

private:
	ofParameter<T> & parameter;
	T oldValue;
	T newValue;
};
