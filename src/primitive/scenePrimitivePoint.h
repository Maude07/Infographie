#pragma once
#include "scenePrimitive.h"
#include "ofMain.h"

class ScenePrimitivePoint : public ScenePrimitive {
private:
	float radius = 4.0f;

public:
	void setRadius(float radius) { radius = std::max(0.0f, radius); }
	float getRadius() const { return radius; }

	void draw() const override;
	ofRectangle getBounds() const override;
	bool contains(float x, float y) const override;
};
