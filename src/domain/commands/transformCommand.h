#pragma once

#include "command.h"
#include "domain/scene/sceneObject.h"
#include "ofMain.h"

struct TransformState {
	ofRectangle bounds;
	float rotation = 0.0f;

	static TransformState of(const SceneObject & object) {
		return { object.getBounds(), object.rotation };
	}

	bool operator==(const TransformState & other) const {
		return bounds == other.bounds && rotation == other.rotation;
	}
	bool operator!=(const TransformState & other) const { return !(*this == other); }
};

class TransformCommand : public Command {
public:
	TransformCommand(SceneObject * target, TransformState oldState, TransformState newState);

	void undo() override;
	void redo() override;

private:
	SceneObject * target;
	TransformState oldState;
	TransformState newState;

	void apply(const TransformState & state);
};
