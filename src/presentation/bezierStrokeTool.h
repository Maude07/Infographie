#pragma once

#include "vector_shapes/sceneShapeBezierCurve.h"

using namespace std;

class BezierStrokeTool {
public:
	void begin(const glm::vec2 & point, ofColor fillColor, ofColor lineColor, float lineWidth,
		const string & name);
	void extend(const glm::vec2 & point);
	unique_ptr<SceneObject> commit(const glm::vec2 & point);
	void cancel();
	bool isActive() const { return stroke != nullptr; }
	void draw() const;

private:
	unique_ptr<SceneShapeBezierCurve> stroke;

	void addPoint(const glm::vec2 & point);
};
