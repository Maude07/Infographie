#includde "scene.h"

void Scene::add(std::unique_ptr<SceneObject> object) {
    objects.push_back(std::move(object));
}

void Scene::draw() const {
    for (const auto & object : objects) {
        object->draw();
    }
}

SceneObject * Scene::hitTest(float x, float y) const {
    for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
        if ((*it)->contains(x, y)) {
            return it->get();
        }
    }
    return nullptr;
}
