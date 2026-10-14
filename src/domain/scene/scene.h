#pragma once
#include "domain/scene/sceneObject.h"
#include "domain/shapes/3d/sceneModel3d.h"
#include "domain/camera/sceneCamera.h"
#include <unordered_set>

using namespace std;

class Scene {
public:
    void add(unique_ptr<SceneObject> object);
	void remove(const unordered_set<SceneObject *> & toRemove);
	unique_ptr<SceneObject> extract(const SceneObject * object);
    void draw(SceneCamera* camera) const;
	void clear();

    SceneObject * hitTest(float x, float y) const;

	SceneCamera * camera = nullptr;
    
    const vector<unique_ptr<SceneObject>> & getObjects() const { return objects; }
    size_t size() const { return objects.size(); }

private:
    vector<unique_ptr<SceneObject>> objects;
};
