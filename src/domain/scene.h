#pragma once
#include "domain/sceneObject.h"

using namespace std;

class Scene {
public:
    void add(unique_ptr<SceneObject> object);
    void remove(const SceneObject * object);
	unique_ptr<SceneObject> extract(const SceneObject * object);
    void draw() const;
	void clear();

    SceneObject * hitTest(float x, float y) const;
    
    const vector<unique_ptr<SceneObject>> & getObjects() const { return objects; }
    size_t size() const { return objects.size(); }

private:
    vector<unique_ptr<SceneObject>> objects;
};
