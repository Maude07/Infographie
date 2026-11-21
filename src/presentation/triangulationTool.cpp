#include "presentation/triangulationTool.h"

namespace {
	constexpr float minPointSpacing = 2.0f;
	constexpr float pointRadius = 4.0f;
}

void TriangulationTool::addPoint(const glm::vec2 & point) {
	if (!points.empty() && glm::distance(points.back(), point) < minPointSpacing) {
		return;
	}
	points.push_back(point);
}

void TriangulationTool::removeLastPoint() {
	if (!points.empty()) {
		points.pop_back();
	}
}

void TriangulationTool::cancel() {
	points.clear();
}

unique_ptr<ScenePrimitiveTriangulation> TriangulationTool::commit() {
	auto mesh = make_unique<ScenePrimitiveTriangulation>();
	if (!mesh->build(points)) {
		return nullptr;
	}
	points.clear();
	return mesh;
}

void TriangulationTool::draw(const ofColor & color) const {
	ofPushStyle();
	ofFill();
	ofSetColor(color);
	for (const glm::vec2 & p : points) {
		ofDrawCircle(p.x, p.y, pointRadius);
	}
	ofPopStyle();
}
