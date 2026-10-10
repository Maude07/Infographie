#include "presentation/bezierStrokeTool.h"

void BezierStrokeTool::addPoint(const glm::vec2 & point) {
	if (!stroke) return;
	stroke->addVertex({ point.x, point.y, 0.0f });
}

void BezierStrokeTool::begin(const glm::vec2 & point, ofColor fillColor, ofColor lineColor, float lineWidth,
	const string & name) {
	stroke = make_unique<SceneShapeBezierCurve>();
	stroke->name = name;
	stroke->filled = false;
	stroke->fillColor = fillColor;
	stroke->lineColor = lineColor;
	stroke->lineWidth = lineWidth;
	stroke->startStroke();
	addPoint(point);
}

void BezierStrokeTool::extend(const glm::vec2 & point) {
	addPoint(point);
}

unique_ptr<SceneObject> BezierStrokeTool::commit(const glm::vec2 & point) {
	if (!stroke) return nullptr;

	addPoint(point);
	stroke->finishStroke();

	unique_ptr<SceneObject> result;
	if (stroke->hasStroke()) {
		result = move(stroke);
	}
	stroke.reset();
	return result;
}

void BezierStrokeTool::cancel() {
	stroke.reset();
}

void BezierStrokeTool::draw() const {
	if (stroke) stroke->draw();
}
