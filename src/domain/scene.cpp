#include "ofMain.h"
#include "domain/scene.h"

using namespace std;

void Scene::add(unique_ptr<SceneObject> object) {
    objects.push_back(move(object));
}

void Scene::draw() const {
    for (const auto & object : objects) {
        object->render();
    }
}

void Scene::remove(const SceneObject * object) {
    objects.erase(
    remove_if(objects.begin(), objects.end(),
        [object](const unique_ptr<SceneObject> & candidate) {
        return candidate.get() == object; }),
        objects.end());
}

void Scene::clear() {
	objects.clear();
}

SceneObject * Scene::hitTest(float x, float y) const {
    for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
        if ((*it)->hitTest(x, y)) {
            return it->get();
        }
    }
    return nullptr;
}
