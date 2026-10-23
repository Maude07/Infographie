#pragma once

#include "ofMain.h"
#include <vector>

struct PathFitterError {
public:
	~PathFitterError();
	PathFitterError();
	PathFitterError(int i, double err);
	int index;
	double error;
};

class BezPoint : public ofPolyline {
public:
	~BezPoint();
	BezPoint();
	BezPoint(glm::vec3 pt);
	BezPoint(glm::vec3 pt, glm::vec3 h);
	BezPoint(glm::vec3 pt, glm::vec3 hIn, glm::vec3 hOut, bool abs = false);

	glm::vec3 point;
	glm::vec3 handleIn;
	glm::vec3 handleOut;
	bool isAbsolute;

	bool hasHandleIn();
	bool hasHandleOut();
	// Converts the handles of a Bezier point from relative coordinates to absolute coordinates
	BezPoint handlesAbsolute();
};

class ofxPathFitter {
public:
	// Simplify a Bezier line
	static std::vector<BezPoint> simplify(ofPolyline line, double tolerance = 2.5);
	static std::vector<BezPoint> simplify(std::vector<glm::vec3> pts, bool isClosed, double tolerance = 2.5);

private:
	std::vector<glm::vec3> points;
	bool closed;

	~ofxPathFitter();
	ofxPathFitter();
	ofxPathFitter(std::vector<glm::vec3> pts, bool isClosed);

	// Simplify the Bezier line from an existing ofxPathFitter instance
	std::vector<BezPoint> fit(double error = 2.5);

	static std::vector<BezPoint> handlesAbsolute(std::vector<BezPoint> BezPoints);

	void addCurve(std::vector<BezPoint> & BezPoints, std::vector<glm::vec3> curve);
	void fitCubic(std::vector<BezPoint> & BezPoints, double error, int first, int last, glm::vec3 tan1, glm::vec3 tan2);
	std::vector<glm::vec3> generateBezier(int first, int last, std::vector<double> uPrime, glm::vec3 tan1, glm::vec3 tan2);
	bool reparameterize(int first, int last, std::vector<double> uPrime, std::vector<glm::vec3> curve);
	double findRoot(std::vector<glm::vec3> curve, glm::vec3 point, double u);
	glm::vec3 evaluate(int degree, std::vector<glm::vec3> curve, double t);
	std::vector<double> chordLengthParameterize(int first, int last);
	PathFitterError findMaxError(int first, int last, std::vector<glm::vec3> curve, std::vector<double> uPrime);

	static glm::vec3 subtract(glm::vec3 p1, glm::vec3 p2);
	static glm::vec3 add(glm::vec3 p1, glm::vec3 p2);
	static glm::vec3 multiply(glm::vec3 p1, double p2);
	static glm::vec3 normalize(glm::vec3 p1, double length);
};
