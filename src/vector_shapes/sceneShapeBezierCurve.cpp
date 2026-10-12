#include "vector_shapes/sceneShapeBezierCurve.h"

namespace {
	constexpr float kMinVertexSpacing = 1.0f;
	constexpr float kMinBounds = 1.0f;
}

void SceneShapeBezierCurve::clear() {
	displayLine.clear();
	bezLine.clear();
	localized = false;
	isClosed = false;
}

void SceneShapeBezierCurve::draw() const {
	ofPushStyle();
	applyStyle();
	ofPushMatrix();
	if (localized) {
		ofTranslate(position.x, position.y);
	}
	displayLine.draw();
	ofPopMatrix();
	ofPopStyle();
}

ofRectangle SceneShapeBezierCurve::getBounds() const {
	if (localized) {
		return ofRectangle(position.x, position.y, size.x, size.y);
	}
	if (displayLine.size() == 0) {
		return ofRectangle(position.x, position.y, 0, 0);
	}
	return displayLine.getBoundingBox();
}

void SceneShapeBezierCurve::setBounds(const ofRectangle & bounds) {
	if (!localized) bakeToLocal();

	float sx = size.x > 0.0f ? bounds.width / size.x : 1.0f;
	float sy = size.y > 0.0f ? bounds.height / size.y : 1.0f;

	for (auto & vertex : displayLine.getVertices()) {
		vertex.x *= sx;
		vertex.y *= sy;
	}
	for (auto & bezierPoint : bezLine) {
		bezierPoint.point.x *= sx;
		bezierPoint.point.y *= sy;
		bezierPoint.handleIn.x *= sx;
		bezierPoint.handleIn.y *= sy;
		bezierPoint.handleOut.x *= sx;
		bezierPoint.handleOut.y *= sy;
	}

	position = { bounds.x, bounds.y };
	size = { bounds.width, bounds.height };
}

bool SceneShapeBezierCurve::contains(float x, float y) const {
	if (displayLine.size() == 0) return false;

	glm::vec3 query(x, y, 0.0f);
	if (localized) {
		query.x -= position.x;
		query.y -= position.y;
	}
	glm::vec3 closest = displayLine.getClosestPoint(query);
	float tolerance = std::max(lineWidth, kClickPadding);
	return glm::distance(closest, query) <= tolerance;
}

void SceneShapeBezierCurve::drawActive(int smoothness) {
	SceneShapeBezierCurve displayStroke = *this;
	if (smoothness > 0) {
		displayStroke.displayLine = displayLine.getSmoothed(smoothness);
	}
	displayStroke.draw();
}

void SceneShapeBezierCurve::drawEditableVertex(BezPoint bezierPoint) {
	ofSetColor(255);
	ofDrawCircle(bezierPoint.point, 5);
	ofNoFill();
	bezierPoint = bezierPoint.handlesAbsolute();
	if (bezierPoint.hasHandleIn()) {
		ofDrawLine(bezierPoint.point, bezierPoint.handleIn);
		ofDrawCircle(bezierPoint.handleIn, 5);
	}
	if (bezierPoint.hasHandleOut()) {
		ofDrawLine(bezierPoint.point, bezierPoint.handleOut);
		ofDrawCircle(bezierPoint.handleOut, 5);
	}
	ofFill();
	ofSetColor(100);
}

void SceneShapeBezierCurve::drawEditable(int iSelectedVertex) {
	ofPushMatrix();
	if (localized) {
		ofTranslate(position.x, position.y);
	}
	ofSetColor(100);
	for (int i = 0; i < static_cast<int>(bezLine.size()); i++) {
		BezPoint bezierPoint = bezLine[i];
		if (i != iSelectedVertex) {
			ofDrawCircle(bezierPoint.point, 5);
		}
	}
	if (iSelectedVertex >= 0 && iSelectedVertex < static_cast<int>(bezLine.size())) {
		drawEditableVertex(bezLine[iSelectedVertex]);
	}
	ofPopMatrix();
}

void SceneShapeBezierCurve::startStroke() {
	clear();
	filled = false;
}

void SceneShapeBezierCurve::addVertex(glm::vec3 v) {
	if (localized) {
		v.x -= position.x;
		v.y -= position.y;
	}
	if (displayLine.size() > 0) {
		glm::vec3 last = displayLine.getVertices().back();
		if (glm::distance(last, v) < kMinVertexSpacing) return;
	}
	displayLine.addVertex(v);
}

void SceneShapeBezierCurve::finishStroke(int smoothness) {
	if (displayLine.size() < 2) return;

	if (smoothness > 0) {
		displayLine = displayLine.getSmoothed(smoothness);
	}
	displayLine.simplify(kNormalizedSimplify);
	updateBezLine();
	snapEndpoints();
	bakeToLocal();
}

bool SceneShapeBezierCurve::hasStroke() const {
	if (displayLine.size() < 2) return false;
	return displayLine.getPerimeter() >= kMinVertexSpacing;
}

void SceneShapeBezierCurve::bakeToLocal() {
	if (localized || displayLine.size() == 0) return;

	ofRectangle box = displayLine.getBoundingBox();
	translateGeometry({ -box.x, -box.y, 0.0f });
	position = { box.x, box.y };
	size = { std::max(box.width, kMinBounds), std::max(box.height, kMinBounds) };
	localized = true;
}

void SceneShapeBezierCurve::translateGeometry(const glm::vec3 & delta) {
	for (auto & vertex : displayLine.getVertices()) {
		vertex += delta;
	}
	for (auto & bezierPoint : bezLine) {
		bezierPoint.point += delta;
	}
}

void SceneShapeBezierCurve::snapEndpoints() {
	isClosed = false;
	if (bezLine.size() <= 2) return;
	glm::vec3 endpoint1 = displayLine.getVertices().front();
	glm::vec3 endpoint2 = displayLine.getVertices().back();

	if (glm::distance(endpoint1, endpoint2) <= kSnapPadding) {
		isClosed = true;
		BezPoint lastPt = bezLine.back();
		bezLine.pop_back();
		bezLine[0].point = lastPt.point;
		bezLine[0].handleIn = lastPt.handleIn;
		updateDisplayLine();
	}
}

bool SceneShapeBezierCurve::snapToIntersection(BezPoint & bezierPoint, glm::vec3 & pt2) {
	glm::vec3 intersection = glm::vec3(-1, -1, -1);
	if (intersectLineToStroke(bezierPoint.point, glm::normalize(pt2) * kSnapPadding, intersection)) {
		bezierPoint.point = intersection;
		return true;
	}
	return false;
}

bool SceneShapeBezierCurve::intersectLineToStroke(glm::vec3 lineStart, glm::vec3 lineDelta, glm::vec3 & intersection) {
	if (displayLine.size() < 2) return false;
	glm::vec3 lineEnd = lineStart - lineDelta;
	auto iterator = displayLine.getVertices().begin();
	while (iterator != displayLine.getVertices().end() - 1) {
		glm::vec3 v1 = *iterator;
		glm::vec3 v2 = *(iterator + 1);
		if (lineStart != v1 && lineStart != v2
			&& lineEnd != v1 && lineEnd != v2
			&& ofLineSegmentIntersection(lineStart, lineEnd, v1, v2, intersection))
			return true;
		iterator++;
	}
	return false;
}

void SceneShapeBezierCurve::updateBezLine() {
	if (displayLine.size() < 2) {
		bezLine.clear();
		return;
	}
	bezLine = ofxPathFitter::simplify(displayLine, 1);
}

void SceneShapeBezierCurve::updateDisplayLine(bool simplify) {
	if (bezLine.empty()) return;

	ofPolyline newDisplayLine;
	newDisplayLine.addVertex(bezLine.front().point);
	for (int i = 1; i < static_cast<int>(bezLine.size()); i++) {
		BezPoint p1 = bezLine[i - 1].handlesAbsolute();
		BezPoint p2 = bezLine[i].handlesAbsolute();
		newDisplayLine.bezierTo(p1.handleOut, p2.handleIn, p2.point);
	}

	if (isClosed) {
		newDisplayLine.close();
		BezPoint p1 = bezLine.back().handlesAbsolute();
		BezPoint p2 = bezLine[0].handlesAbsolute();
		newDisplayLine.bezierTo(p1.handleOut, p2.handleIn, p2.point);
	}

	displayLine = newDisplayLine;
	if (simplify) {
		displayLine.simplify(kNormalizedSimplify);
	}
}

void SceneShapeBezierCurve::modifyHandle(int id, VertexTypes selectedHandle, int x, int y) {
	glm::vec3 pt = bezLine[id].point;
	float localX = localized ? x - position.x : static_cast<float>(x);
	float localY = localized ? y - position.y : static_cast<float>(y);
	if (selectedHandle == VertexTypes::HANDLEIN) {
		bezLine[id].handleIn.x = localX - pt.x;
		bezLine[id].handleIn.y = localY - pt.y;
	} else {
		bezLine[id].handleOut.x = localX - pt.x;
		bezLine[id].handleOut.y = localY - pt.y;
	}
	updateDisplayLine();
}

void SceneShapeBezierCurve::modifyVertex(int id, int x, int y) {
	BezPoint * pt = &bezLine[id];
	pt->point.x = localized ? x - position.x : static_cast<float>(x);
	pt->point.y = localized ? y - position.y : static_cast<float>(y);
	updateDisplayLine();
}

int SceneShapeBezierCurve::getSelectedVertex(int iSelectedVertex, VertexTypes & handle, int x, int y) {
	handle = VertexTypes::INVALID;
	float localX = localized ? x - position.x : static_cast<float>(x);
	float localY = localized ? y - position.y : static_cast<float>(y);
	int nPoints = static_cast<int>(bezLine.size());
	for (int i = 0; i < nPoints; i++) {
		BezPoint s = bezLine[i].handlesAbsolute();
		if (iSelectedVertex == i) {
			if ((isClosed || i != 0) && glm::distance(glm::vec2(s.handleIn.x, s.handleIn.y), glm::vec2(localX, localY)) < kClickPadding) {
				handle = VertexTypes::HANDLEIN;
			} else if ((isClosed || i != nPoints - 1) && glm::distance(glm::vec2(s.handleOut.x, s.handleOut.y), glm::vec2(localX, localY)) < kClickPadding) {
				handle = VertexTypes::HANDLEOUT;
			}
		}

		if (handle == VertexTypes::INVALID && glm::distance(glm::vec2(s.point.x, s.point.y), glm::vec2(localX, localY)) < kClickPadding) {
			handle = VertexTypes::POINT;
		}

		if (handle != VertexTypes::INVALID) {
			return i;
		}
	}
	return -1;
}

int distLine(glm::vec3 pt, ofPolyline * line) {
	glm::vec3 cl = (*line).getClosestPoint(pt);
	return static_cast<int>(glm::distance(cl, pt));
}

int SceneShapeBezierCurve::getSelectedStroke(std::vector<SceneShapeBezierCurve> * strokes, glm::vec3 mousePos) {
	for (int i = 0; i < static_cast<int>((*strokes).size()); i++) {
		ofPolyline * displayLine = &((*strokes)[i].displayLine);
		glm::vec3 query = mousePos;
		if ((*strokes)[i].localized) {
			query.x -= (*strokes)[i].position.x;
			query.y -= (*strokes)[i].position.y;
		}
		if (distLine(query, displayLine) < kClickPadding) {
			return i;
		}
	}
	return -1;
}

void SceneShapeBezierCurve::translateLine(glm::vec3 lastMousePos, int x, int y) {
	int xShift = x - lastMousePos.x;
	int yShift = y - lastMousePos.y;
	if (localized) {
		position.x += xShift;
		position.y += yShift;
		return;
	}
	translateGeometry({ static_cast<float>(xShift), static_cast<float>(yShift), 0.0f });
}
