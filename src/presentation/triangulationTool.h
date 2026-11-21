#pragma once

#include "ofMain.h"
#include "domain/shapes/2d/scenePrimitiveTriangulation.h"

using namespace std;

class TriangulationTool {
public:
	void addPoint(const glm::vec2 & point);
	void removeLastPoint();
	void cancel();

	bool hasPoints() const { return !points.empty(); }
	size_t pointCount() const { return points.size(); }

	unique_ptr<ScenePrimitiveTriangulation> commit();

	void draw(const ofColor & color) const;

private:
	vector<glm::vec2> points;
};
