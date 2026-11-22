#include "domain/shapes/2d/scenePrimitiveTriangulation.h"
#include "domain/geometry/delaunay.h"

bool ScenePrimitiveTriangulation::build(const std::vector<glm::vec2> & points) {
	Delaunay delaunay;
	std::vector<std::array<int, 3>> result = delaunay.triangulate(points);
	if (result.empty()) {
		return false;
	}

	vertices = points;
	triangles = std::move(result);
	rebuildMesh();
	syncBounds();
	return true;
}

void ScenePrimitiveTriangulation::draw() const {
	ofPushStyle();

	if (filled) {
		ofFill();
		ofSetColor(fillColor);
		mesh.draw();
	}

	ofSetLineWidth(lineWidth);
	ofSetColor(lineColor);
	mesh.drawWireframe();

	ofFill();
	float radius = std::max(lineWidth * 1.5f, 2.5f);
	for (const glm::vec2 & v : vertices) {
		ofDrawCircle(v.x, v.y, radius);
	}

	ofPopStyle();
}

ofRectangle ScenePrimitiveTriangulation::getBounds() const {
	if (vertices.empty()) {
		return ofRectangle(position.x, position.y, 0, 0);
	}
	float minX = vertices[0].x, maxX = vertices[0].x;
	float minY = vertices[0].y, maxY = vertices[0].y;
	for (const glm::vec2 & v : vertices) {
		minX = std::min(minX, v.x); maxX = std::max(maxX, v.x);
		minY = std::min(minY, v.y); maxY = std::max(maxY, v.y);
	}
	return ofRectangle(minX, minY, maxX - minX, maxY - minY);
}

void ScenePrimitiveTriangulation::setBounds(const ofRectangle & bounds) {
	ofRectangle old = getBounds();
	for (glm::vec2 & v : vertices) {
		float tx = old.width > 0.0f ? (v.x - old.x) / old.width : 0.0f;
		float ty = old.height > 0.0f ? (v.y - old.y) / old.height : 0.0f;
		v = glm::vec2(bounds.x + tx * bounds.width, bounds.y + ty * bounds.height);
	}
	rebuildMesh();
	syncBounds();
}

bool ScenePrimitiveTriangulation::contains(float x, float y) const {
	glm::vec2 p(x, y);
	auto orient = [](const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c) {
		return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
	};

	for (const auto & t : triangles) {
		const glm::vec2 & a = vertices[t[0]];
		const glm::vec2 & b = vertices[t[1]];
		const glm::vec2 & c = vertices[t[2]];
		if (orient(a, b, p) >= 0 && orient(b, c, p) >= 0 && orient(c, a, p) >= 0) {
			return true;
		}
	}
	return false;
}

void ScenePrimitiveTriangulation::rebuildMesh() {
	mesh.clear();
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
	for (const glm::vec2 & v : vertices) {
		mesh.addVertex(glm::vec3(v, 0.0f));
	}
	for (const auto & t : triangles) {
		mesh.addTriangle(t[0], t[1], t[2]);
	}
}

void ScenePrimitiveTriangulation::syncBounds() {
	ofRectangle bounds = getBounds();
	position = { bounds.x, bounds.y };
	size = { bounds.width, bounds.height };
}
