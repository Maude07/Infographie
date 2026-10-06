#include "addObjectCommand.h"

using namespace std;

AddObjectCommand::AddObjectCommand(Scene * scene, unique_ptr<SceneObject> object)
	: scene(scene)
	, ownedObject(move(object))
	, rawObject(ownedObject.get()) { }

void AddObjectCommand::redo() {
	if (ownedObject) {
		scene->add(move(ownedObject));
	}
}

void AddObjectCommand::undo() {
	ownedObject = scene->extract(rawObject);
}
