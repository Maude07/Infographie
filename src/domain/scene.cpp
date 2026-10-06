#include "ofMain.h"
#include "domain/scene.h"

using namespace std;

void Scene::add(unique_ptr<SceneObject> object) {
    objects.push_back(move(object));
}

void Scene::draw() const {
    for (const auto & object : objects) {
        object->draw();
    }
}

unique_ptr<SceneObject> Scene::extract(const SceneObject * object) {
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if (it->get() == object) {
			unique_ptr<SceneObject> extracted = move(*it);
			objects.erase(it);
			return extracted;
		}
	}
	return nullptr;
}

void Scene::remove(const SceneObject * object) {
	extract(object);
}

void Scene::clear() {
	objects.clear();
}

SceneObject * Scene::hitTest(float x, float y) const {
    for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
        if ((*it)->contains(x, y)) {
            return it->get();
        }
    }
    return nullptr;
}
