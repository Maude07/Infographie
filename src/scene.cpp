#pragma once
#include "ofMain.h"
#include "scene.h"
#include "sceneObject.h"

using namespace std;

void Scene::add(unique_ptr<SceneObject> object) {
    objects.push_back(move(object));
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
