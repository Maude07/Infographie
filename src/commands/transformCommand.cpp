
#include "transformCommand.h"

TransformCommand::TransformCommand(SceneObject * target, ofRectangle oldBounds, ofRectangle newBounds)
	: target(target)
	, oldBounds(oldBounds)
	, newBounds(newBounds) {
}

void TransformCommand::undo() {
	if (target) target->setBounds(oldBounds);
}

void TransformCommand::redo() {
	if (target) target->setBounds(newBounds);
}
