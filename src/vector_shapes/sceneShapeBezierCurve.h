#pragma once
#include "scenePrimitive.h"
#include "ofMain.h"
#include "ofxPathFitter.h"


#define CLICKPADDING 10
#define SNAPPADDING 20


class SceneShapeBezierCurve : public ScenePrimitive {

		void drawEditableVertex(BezPoint vertex);

public:
		enum VertexTypes {
			INVALID,
			POINT,
			HANDLEIN,
			HANDLEOUT,
		};
		const float normalizedSimplify = 0.5f;


		SceneShapeBezierCurve();
		~SceneShapeBezierCurve();
		SceneShapeBezierCurve(int i);

		bool isClosed;

		void clear();

		void draw() const override;
		void drawActive(int smoothness);
		void drawEditable(int iSelectedVertex);

		void startStroke();
		void addVertex(glm::vec3 v);
		void finishStroke(int smoothness);

		void updateBezLine();
		void updateDisplayLine(bool simplify = true);
		void modifyHandle(int id, VertexTypes selectedHandle, int x, int y);
		void modifyVertex(int id, int x, int y);

		int getSelectedVertex(int iSelectedVertex, VertexTypes & handle, int x, int y);
		static int getSelectedStroke(vector<SceneShapeBezierCurve> * strokes, glm::vec3 mousePos);

		void translateLine(glm::vec3 lastMousePos, int x, int y);

		void snapEndpoints();
		bool intersectLineToStroke(glm::vec3 lineStart, glm::vec3 lineDelta, glm::vec3 & intersection);
		bool snapToIntersection(BezPoint & pt, glm::vec3 & pt2);

		ofPolyline displayLine;
		vector<BezPoint> bezLine;



};
