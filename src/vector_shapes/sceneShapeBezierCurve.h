#pragma once
#include "domain/shapes/2d/scenePrimitive.h"
#include "ofMain.h"
#include "ofxPathFitter.h"

class SceneShapeBezierCurve : public ScenePrimitive {
	void drawEditableVertex(BezPoint vertex);
	void bakeToLocal();
	void translateGeometry(const glm::vec3 & delta);

public:
	enum VertexTypes {
		INVALID,
		POINT,
		HANDLEIN,
		HANDLEOUT,
	};

	static constexpr float kNormalizedSimplify = 0.5f;
	static constexpr float kClickPadding = 10.0f;
	static constexpr float kSnapPadding = 20.0f;
	static constexpr int kStrokeSmoothness = 2;

	SceneShapeBezierCurve() = default;

	bool isClosed = false;

	void clear();

	void draw() const override;
	ofRectangle getBounds() const override;
	void setBounds(const ofRectangle & bounds) override;
	bool contains(float x, float y) const override;

	void drawActive(int smoothness);
	void drawEditable(int iSelectedVertex);

	void startStroke();
	void addVertex(glm::vec3 v);
	void finishStroke(int smoothness = kStrokeSmoothness);
	bool hasStroke() const;

	void updateBezLine();
	void updateDisplayLine(bool simplify = true);
	void modifyHandle(int id, VertexTypes selectedHandle, int x, int y);
	void modifyVertex(int id, int x, int y);

	int getSelectedVertex(int iSelectedVertex, VertexTypes & handle, int x, int y);
	static int getSelectedStroke(std::vector<SceneShapeBezierCurve> * strokes, glm::vec3 mousePos);

	void translateLine(glm::vec3 lastMousePos, int x, int y);

	void snapEndpoints();
	bool intersectLineToStroke(glm::vec3 lineStart, glm::vec3 lineDelta, glm::vec3 & intersection);
	bool snapToIntersection(BezPoint & bezierPoint, glm::vec3 & pt2);

	ofPolyline displayLine;
	std::vector<BezPoint> bezLine;

private:
	bool localized = false;
};
