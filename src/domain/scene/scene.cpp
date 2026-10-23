#include "ofMain.h"
#include "domain/scene/scene.h"

using namespace std;

void Scene::add(unique_ptr<SceneObject> object) {
    objects.push_back(move(object));
}

void Scene::draw(SceneCamera* camera) const {
	glClear(GL_DEPTH_BUFFER_BIT);
    for (const auto & object : objects) {
		if (!isVisible(object.get())) continue;
		bool is3d = dynamic_cast<const SceneObject3d *>(object.get()) != nullptr;
		if (is3d && camera) {
			camera->begin();
			object->render();
			camera->end();
		} else {
			object->render();
		}
    }
}

unique_ptr<SceneObject> Scene::extract(const SceneObject * object) {
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if (it->get() == object) {
			isolated.erase(it->get());
			unique_ptr<SceneObject> extracted = move(*it);
			objects.erase(it);
			return extracted;
		}
	}
	return nullptr;
}

void Scene::remove(const unordered_set<SceneObject*> & toRemove) {
	for (SceneObject * object : toRemove) isolated.erase(object);
	objects.erase(
		remove_if(objects.begin(), objects.end(),
			[&toRemove](const unique_ptr<SceneObject> & o) {
				return toRemove.count(o.get()) > 0;
			}),
		objects.end());
}

void Scene::clear() {
	objects.clear();
	isolated.clear();
}

SceneObject * Scene::hitTest(float x, float y) const {
    for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
        if (isVisible(it->get()) && (*it)->hitTest(x, y)) {
            return it->get();
        }
    }
    return nullptr;
}
