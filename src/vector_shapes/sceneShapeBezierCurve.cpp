#include "sceneShapeBezierCurve.h"

SceneShapeBezierCurve::SceneShapeBezierCurve() {
}

SceneShapeBezierCurve::~SceneShapeBezierCurve() {
}

SceneShapeBezierCurve::SceneShapeBezierCurve(int i) {
}

void SceneShapeBezierCurve::clear() {
	applyStyle();

	displayLine.clear();
	bezLine.clear();
}

void SceneShapeBezierCurve::draw() const {
	displayLine.draw();
}

void SceneShapeBezierCurve::drawActive(int smoothness) {
	SceneShapeBezierCurve displayStroke = *this;
	if (smoothness > 0) {
		displayStroke.displayLine = displayLine.getSmoothed(smoothness);
	}
	displayStroke.draw();
}

void SceneShapeBezierCurve::drawEditableVertex(BezPoint pt) {
	ofSetColor(255);
	ofDrawCircle(pt.point, 5);
	ofNoFill();
	pt = pt.handlesAbsolute();
	if (pt.hasHandleIn()) {
		ofDrawLine(pt.point, pt.handleIn);
		ofDrawCircle(pt.handleIn, 5);
	}
	if (pt.hasHandleOut()) {
		ofDrawLine(pt.point, pt.handleOut);
		ofDrawCircle(pt.handleOut, 5);
	}
	ofFill();
	ofSetColor(100);
}

void SceneShapeBezierCurve::drawEditable(int iSelectedVertex) {
	ofSetColor(100);
	for (int i = 0; i < bezLine.size(); i++) {
		BezPoint pt = bezLine[i];
		if (i != iSelectedVertex) {
			ofDrawCircle(pt.point, 5);
		}
	}
	if (iSelectedVertex >= 0 && iSelectedVertex < bezLine.size())
		drawEditableVertex(bezLine[iSelectedVertex]);
}

void SceneShapeBezierCurve::startStroke() {
}

void SceneShapeBezierCurve::addVertex(glm::vec3 v) {
	displayLine.addVertex(v);
}

void SceneShapeBezierCurve::finishStroke(int smoothness) {
	displayLine = displayLine.getSmoothed(smoothness);
	displayLine.simplify(normalizedSimplify);
	updateBezLine();
	snapEndpoints();
}

void SceneShapeBezierCurve::snapEndpoints() {
	isClosed = false;
	if (bezLine.size() <= 2) return;
	glm::vec3 endpoint1 = displayLine[0];
	glm::vec3 endpoint2 = *(displayLine.end() - 1);

	if (glm::distance(endpoint1, endpoint2) <= SNAPPADDING) {
		isClosed = true;
		BezPoint lastPt = bezLine.back();
		bezLine.pop_back();
		bezLine[0].point = lastPt.point;
		bezLine[0].handleIn = lastPt.handleIn;
		updateDisplayLine();
		return;
	}

}

bool SceneShapeBezierCurve::snapToIntersection(BezPoint & pt, glm::vec3 & pt2) {
	glm::vec3 intersection = glm::vec3(-1, -1, -1);
	if (intersectLineToStroke(pt.point, glm::normalize(pt2) * (float)SNAPPADDING, intersection)) {
		pt.point = intersection;
		return true;
	}
	return false;
}

bool SceneShapeBezierCurve::intersectLineToStroke(glm::vec3 lineStart, glm::vec3 lineDelta, glm::vec3 & intersection) {
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
	bezLine = ofxPathFitter::simplify(displayLine, 1);
}

void SceneShapeBezierCurve::updateDisplayLine(bool simplify) {
	ofPolyline newDisplayLine;
	newDisplayLine.addVertex(bezLine.front().point);
	for (int i = 1; i < bezLine.size(); i++) {
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
		displayLine.simplify(normalizedSimplify);
	}
}

void SceneShapeBezierCurve::modifyHandle(int id, VertexTypes selectedHandle, int x, int y) {
	glm::vec3 pt = bezLine[id].point;
	if (selectedHandle == VertexTypes::HANDLEIN) {
		bezLine[id].handleIn.x = x - pt.x;
		bezLine[id].handleIn.y = y - pt.y;
	} else {
		bezLine[id].handleOut.x = x - pt.x;
		bezLine[id].handleOut.y = y - pt.y;
	}
	updateDisplayLine();
}

void SceneShapeBezierCurve::modifyVertex(int id, int x, int y) {
	BezPoint * pt = &bezLine[id];
	pt->point.x = x;
	pt->point.y = y;
	updateDisplayLine();
}

int SceneShapeBezierCurve::getSelectedVertex(int iSelectedVertex, VertexTypes & handle, int x, int y) {
	handle = VertexTypes::INVALID;
	int nPoints = bezLine.size();
	for (int i = 0; i < nPoints; i++) {
		BezPoint s = bezLine[i].handlesAbsolute();
		if (iSelectedVertex == i) {
			if ((isClosed || i != 0) && glm::distance(glm::vec2(s.handleIn.x, s.handleIn.y), glm::vec2(x, y)) < CLICKPADDING) {
				handle = VertexTypes::HANDLEIN;
			} else if ((isClosed || i != nPoints - 1) && glm::distance(glm::vec2(s.handleOut.x, s.handleOut.y), glm::vec2(x, y)) < CLICKPADDING) {
				handle = VertexTypes::HANDLEOUT;
			}
		}

		if (handle == VertexTypes::INVALID && glm::distance(glm::vec2(s.point.x, s.point.y), glm::vec2(x, y)) < CLICKPADDING) {
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
	return glm::distance(cl, pt);
}

int SceneShapeBezierCurve::getSelectedStroke(vector<SceneShapeBezierCurve> * strokes, glm::vec3 mousePos) {
	for (int i = 0; i < (*strokes).size(); i++) {
		ofPolyline * displayLine = &((*strokes)[i].displayLine);
		if (distLine(mousePos, displayLine) < CLICKPADDING) {
			return i;
		}
	}
	return -1;
}

void SceneShapeBezierCurve::translateLine(glm::vec3 lastMousePos, int x, int y) {
	int xShift = x - lastMousePos.x;
	int yShift = y - lastMousePos.y;
	for (int i = 0; i < bezLine.size(); i++) {
		BezPoint * pt = &bezLine[i];
		pt->point.x += xShift;
		pt->point.y += yShift;
	}
	for (int i = 0; i < displayLine.size(); i++) {
		displayLine[i].x += xShift;
		displayLine[i].y += yShift;
	}
}
