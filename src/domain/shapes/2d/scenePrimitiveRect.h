#pragma once
#include "domain/shapes/2d/scenePrimitive.h"
#include "ofMain.h"

class ScenePrimitiveRect : public ScenePrimitive {
public:
	void draw() const override;
};
