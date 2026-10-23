#pragma once

#include "command.h"
#include "ofMain.h"

class ColorCommand : public Command {
public:
	ColorCommand(ofColor & target, const ofColor & oldValue, const ofColor & newValue)
		: target(target)
		, oldValue(oldValue)
		, newValue(newValue) { }

	void undo() override { target = oldValue; }
	void redo() override { target = newValue; }

private:
	ofColor & target;
	ofColor oldValue;
	ofColor newValue;
};
