#include "domain/commands/transformCommand.h"

TransformCommand::TransformCommand(SceneObject * target, ofRectangle oldBounds, ofRectangle newBounds)
	: target(target)
	, oldBounds(oldBounds)
	, newBounds(newBounds) { }

void TransformCommand::undo() {
	target->setBounds(oldBounds);
}

void TransformCommand::redo() {
	target->setBounds(newBounds);
}
