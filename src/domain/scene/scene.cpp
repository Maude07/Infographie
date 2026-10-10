#include "ofMain.h"
#include "domain/scene/scene.h"

using namespace std;

void Scene::add(unique_ptr<SceneObject> object) {
    objects.push_back(move(object));
}

void Scene::draw() const {
    for (const auto & object : objects) {
        object->draw();
    }
}

void Scene::remove(const unordered_set<SceneObject*> & toRemove) {
	objects.erase(
		remove_if(objects.begin(), objects.end(),
			[&toRemove](const unique_ptr<SceneObject> & o) {
				return toRemove.count(o.get()) > 0;
			}),
		objects.end());
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
