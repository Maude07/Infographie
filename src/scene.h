#pragma once
#include "sceneObject.h"

using namespace std;

class Scene {
public:
    void add(unique_ptr<SceneObject> object);
    void draw() const;

    SceneObject * hitTest(float x, float y) const;
    
    size_t size() const { return objects.size(); }

private:
    vector<unique_ptr<SceneObject>> objects;
};
