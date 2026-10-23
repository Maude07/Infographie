#include "domain/commands/transformCommand.h"

TransformCommand::TransformCommand(SceneObject * target, TransformState oldState, TransformState newState)
	: target(target)
	, oldState(oldState)
	, newState(newState) { }

void TransformCommand::undo() {
	apply(oldState);
}

void TransformCommand::redo() {
	apply(newState);
}

void TransformCommand::apply(const TransformState & state) {
	target->setBounds(state.bounds);
	target->rotation = state.rotation;
}
