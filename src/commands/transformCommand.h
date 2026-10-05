
#pragma once

#include "command.h"
#include "ofMain.h"
#include "domain/sceneObject.h"

class TransformCommand : public Command {
public:
	TransformCommand(SceneObject * target,
		glm::vec2 oldPos, glm::vec2 oldSize,
		glm::vec2 newPos, glm::vec2 newSize)
		: target(target)
		, oldPos(oldPos)
		, oldSize(oldSize)
		, newPos(newPos)
		, newSize(newSize) { }

	void undo() override {
		target->position = oldPos;
		target->size = oldSize;
	}

	void redo() override {
		target->position = newPos;
		target->size = newSize;
	}

private:
	SceneObject * target;
	glm::vec2 oldPos, oldSize;
	glm::vec2 newPos, newSize;
};
