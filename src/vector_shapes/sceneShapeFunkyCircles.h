#pragma once
#include "ofMain.h"
#include "scenePrimitive.h"

class SceneShapeFunkyCircles : public ScenePrimitive {
public:
	void draw() const override;
	ofRectangle getBounds() const override;
	bool contains(float x, float y) const override;
};
