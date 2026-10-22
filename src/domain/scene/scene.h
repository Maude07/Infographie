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

	void isolate(const unordered_set<SceneObject *> & objects) { isolated = objects; }
	void clearIsolation() { isolated.clear(); }
	bool isIsolating() const { return !isolated.empty(); }
	bool isVisible(const SceneObject * object) const {
		return isolated.empty() || isolated.count(const_cast<SceneObject *>(object)) > 0;
	}
	const unordered_set<SceneObject *> & getIsolated() const { return isolated; }

	SceneCamera * camera = nullptr;
    
    const vector<unique_ptr<SceneObject>> & getObjects() const { return objects; }
    size_t size() const { return objects.size(); }

private:
    vector<unique_ptr<SceneObject>> objects;
	unordered_set<SceneObject *> isolated;
};
