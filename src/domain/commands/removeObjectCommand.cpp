#include "domain/commands/removeObjectCommand.h"

using namespace std;

RemoveObjectsCommand::RemoveObjectsCommand(Scene * scene, const unordered_set<SceneObject *> & toRemove)
	: scene(scene) {
	rawObjects.assign(toRemove.begin(), toRemove.end());
}

void RemoveObjectsCommand::redo() {
	ownedObjects.clear();
	for (SceneObject * object : rawObjects) {
		if (auto extracted = scene->extract(object)) {
			ownedObjects.push_back(move(extracted));
		}
	}
}

void RemoveObjectsCommand::undo() {
	for (auto & object : ownedObjects) {
		scene->add(move(object));
	}
	ownedObjects.clear();
}
