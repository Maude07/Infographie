#pragma once
#include "ofMain.h"
#include "scenePrimitive.h"
#include "sceneShapeBezierCurve.h"
class SceneShapeWave : public ScenePrimitive {
public:

	

	void draw() const override;
	ofRectangle getBounds() const override;
	bool contains(float x, float y) const override;

};
