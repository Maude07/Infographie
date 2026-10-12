#pragma once

#include "command.h"
#include "domain/scene/sceneObject.h"
#include "ofMain.h"

class TransformCommand : public Command {
public:
	TransformCommand(SceneObject * target, ofRectangle oldBounds, ofRectangle newBounds);

	void undo() override;
	void redo() override;

private:
	SceneObject * target;
	ofRectangle oldBounds;
	ofRectangle newBounds;
};
