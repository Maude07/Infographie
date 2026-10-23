#pragma once

#include "command.h"
#include "domain/scene/scene.h"
#include "domain/scene/sceneObject.h"
#include <memory>
#include <unordered_set>
#include <vector>

class RemoveObjectsCommand : public Command {
public:
	RemoveObjectsCommand(Scene * scene, const std::unordered_set<SceneObject *> & toRemove);

	void undo() override;
	void redo() override;

private:
	Scene * scene;
	std::vector<SceneObject *> rawObjects;
	std::vector<std::unique_ptr<SceneObject>> ownedObjects;
};
