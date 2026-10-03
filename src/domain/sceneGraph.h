#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "sceneEntity.h"

using namespace std;

class SceneGraph {

public:
	void manageDraw();
	void setUp();
	void addEntityToSceneGraph(shared_ptr<SceneEntity> entity);

	void mousePressed(int x, int y);
	void deleteSelected();


private:
	ofxPanel gui;

	vector<shared_ptr<SceneEntity>> entities;

	const float offsetX = 15;
	const float offsetY = 15;
	const float spacing = 30.0f;
};
