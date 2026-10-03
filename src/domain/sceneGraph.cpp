#include "sceneGraph.h"
#include "sceneObject.h"

using namespace std;

void SceneGraph::manageDraw() {
	for (auto& entity : entities) {
		entity->update();
		entity->drawRow();
	}

}

void SceneGraph::setUp() {


	gui.setup("Scene Graph", "settings.json", 10, 10);

	for (auto & entity : entities) {
		gui.add(entity->getParameters());
	}

}


void SceneGraph::mousePressed(int x, int y) {

	for (auto& entity : entities) {
		if (entity->isMouseInside(x, y)) {
			entity->setIsSelected(true);
		}
		else {
			entity->setIsSelected(false);
		}
	}
}

void SceneGraph::deleteSelected() {

	for (int i = 0; i < entities.size(); i++) {

		if (entities[i]->getIsSelected()) {
			entities[i]->getLinkedObject()->isDeleted = true;
			entities.erase(entities.begin() + i);



			gui.clear();

			for (int j = i; j < entities.size(); j++) {
				glm::vec3 newPosition = entities[j]->getParameters().get<glm::vec3>("Position");
				newPosition.y -= spacing;
				entities[j]->setPosition(newPosition);
			}

		}

		
	}


	for (int i = 0; i < entities.size(); i++)
		gui.add(entities[i]->getParameters());

}

void SceneGraph::addEntityToSceneGraph(shared_ptr<SceneEntity> entity) {

	int index = entities.size();



	entity->setPosition(glm::vec3(offsetY, index * spacing, 0));
	this->entities.push_back(entity);



	gui.add(entity->getParameters());

}
