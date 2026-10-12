#pragma once

#include "command.h"
#include "domain/scene/scene.h"
#include "domain/scene/sceneObject.h"
#include <memory>

class AddObjectCommand : public Command {
public:
	AddObjectCommand(Scene * scene, std::unique_ptr<SceneObject> object);

	void undo() override;
	void redo() override;

private:
	Scene * scene;
	std::unique_ptr<SceneObject> ownedObject;
	SceneObject * rawObject;
};
