#pragma once
#include "domain/sceneObject.h"
#include <unordered_set>

using namespace std;

class Scene {
public:
    void add(unique_ptr<SceneObject> object);
	void remove(const unordered_set<SceneObject *> & toRemove);
    void draw() const;
	void clear();

    SceneObject * hitTest(float x, float y) const;
    
    const vector<unique_ptr<SceneObject>> & getObjects() const { return objects; }
    size_t size() const { return objects.size(); }

private:
    vector<unique_ptr<SceneObject>> objects;
};
