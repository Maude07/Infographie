#include "ofMain.h"
#include "domain/scene/scene.h"

using namespace std;

void Scene::add(unique_ptr<SceneObject> object) {
    objects.push_back(move(object));
}

void Scene::draw(SceneCamera* camera) const {
    for (const auto & object : objects) {
		bool is3d = dynamic_cast<const SceneModel3d *>(object.get()) != nullptr;
		if (is3d && camera) {
			camera->begin();
			object->draw();
			camera->end();
		} else {
			object->draw();
		}
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
