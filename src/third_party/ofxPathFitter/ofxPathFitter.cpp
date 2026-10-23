#include "ofxPathFitter.h"

PathFitterError::~PathFitterError() {}
PathFitterError::PathFitterError() {}
PathFitterError::PathFitterError(int i, double err) {
    index = i;
    error = err;
}

BezPoint::~BezPoint() {}
BezPoint::BezPoint() { isAbsolute = false; }
BezPoint::BezPoint(glm::vec3 pt) {
    point = pt;
	isAbsolute = false;
}
BezPoint::BezPoint(glm::vec3 pt, glm::vec3 h) {
	point = pt;
	handleIn = h;
	isAbsolute = false;
}
BezPoint::BezPoint(glm::vec3 pt, glm::vec3 hIn, glm::vec3 hOut, bool abs) {
    point = pt;
    handleIn = hIn;
	handleOut = hOut;
	isAbsolute = abs;
}
bool BezPoint::hasHandleIn() {
	if (isAbsolute) return handleIn != point;
	return handleIn.x != 0 && handleIn.y != 0;
}
bool BezPoint::hasHandleOut() {
	if (isAbsolute) return handleOut != point;
	return handleOut.x != 0 && handleOut.y != 0;
}

BezPoint BezPoint::handlesAbsolute() {
	return BezPoint(point, handleIn + point, handleOut + point, true);
}

ofxPathFitter::~ofxPathFitter() {}
ofxPathFitter::ofxPathFitter() {}
ofxPathFitter::ofxPathFitter(vector<glm::vec3> pts, bool isClosed) {
    points = pts;
    closed = isClosed;

    if (closed) {
        points.insert(points.begin(), points[points.size() - 1]);
        points.push_back(points[1]);
    }
}

vector<BezPoint> ofxPathFitter::simplify(ofPolyline line, double tolerance) {
	return simplify(line.getVertices(), line.isClosed(), tolerance);
}

vector<BezPoint> ofxPathFitter::simplify(vector<glm::vec3> pts, bool isClosed, double tolerance) {
	ofxPathFitter lineFitter(pts, isClosed);
	return lineFitter.fit(tolerance);
}

vector<BezPoint> ofxPathFitter::fit(double error) {
    int length = points.size();
    vector<BezPoint> BezPoints;
    if (length > 0) {
        BezPoints.push_back(BezPoint(points.front()));
        if (length > 1) {
            fitCubic(BezPoints, error, 0, length - 1,
                    subtract(points[1], points[0]),
                    subtract(points[length - 2], points[length - 1]));
            if (closed) {
                BezPoints.erase(BezPoints.begin());
                BezPoints.pop_back();
            }
        }
    }

    return BezPoints;
}

void ofxPathFitter::addCurve(vector<BezPoint> & BezPoints, vector<glm::vec3> curve) {
	BezPoints[BezPoints.size() - 1].handleOut = subtract(curve[1], curve[0]);
    BezPoints.push_back(BezPoint(curve[3], subtract(curve[2], curve[3])));
}

void ofxPathFitter::fitCubic(vector<BezPoint> & BezPoints, double error, int first, int last, glm::vec3 tan1, glm::vec3 tan2) {
    if (last - first == 1) {
		glm::vec3 pt1 = points[first];
		glm::vec3 pt2 = points[last];
        double dist = glm::distance(pt1, pt2) / 3.0;
		vector<glm::vec3> curve;
        curve.push_back(pt1);
        curve.push_back(add(pt1, normalize(tan1, dist)));
        curve.push_back(add(pt2, normalize(tan2, dist)));
        curve.push_back(pt2);
        addCurve(BezPoints, curve);
        return;
    }
    vector<double> uPrime = chordLengthParameterize(first, last);
	double maxError = max(error, error * error);
    int split;
    bool parametersInOrder = true;
    for (int i = 0; i <= 4; i++) {
		vector<glm::vec3> curve = generateBezier(first, last, uPrime, tan1, tan2);
        PathFitterError max = findMaxError(first, last, curve, uPrime);
        if (max.error < error && parametersInOrder) {
            addCurve(BezPoints, curve);
            return;
        }
        split = max.index;
        if (max.error >= maxError)
            break;
        parametersInOrder = reparameterize(first, last, uPrime, curve);
        maxError = max.error;
    }
	glm::vec3 tanCenter = subtract(points[split - 1], points[split + 1]);
    fitCubic(BezPoints, error, first, split, tan1, tanCenter);
    fitCubic(BezPoints, error, split, last, multiply(tanCenter, -1), tan2);
}

vector<glm::vec3> ofxPathFitter::generateBezier(int first, int last, vector<double> uPrime, glm::vec3 tan1, glm::vec3 tan2) {
    double epsilon = 1e-12;
	glm::vec3 pt1 = points[first];
	glm::vec3 pt2 = points[last];
    double C[2][2] = {0};
    double X[2] = {0};

    int l = last - first + 1;
    for (int i = 0; i < l; i++) {
        double u = uPrime[i];
        double t = 1 - u;
        double b = 3 * u * t;
        double b0 = t * t * t;
        double b1 = b * t;
        double b2 = b * u;
        double b3 = u * u * u;
		glm::vec3 a1 = normalize(tan1, b1);
		glm::vec3 a2 = normalize(tan2, b2);
		glm::vec3 tmp = subtract(subtract(points[first + i], multiply(pt1, b0 + b1)), multiply(pt2, b2 + b3));
        C[0][0] += glm::dot(a1,a1);
        C[0][1] += glm::dot(a1,a2);
        C[1][0] = C[0][1];
        C[1][1] += glm::dot(a2,a2);
        X[0] += glm::dot(a1,tmp);
        X[1] += glm::dot(a2,tmp);
    }

	double detC0C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
	double alpha1;
	double alpha2;
    if (abs(detC0C1) > epsilon) {
        double detC0X = C[0][0] * X[1]    - C[1][0] * X[0];
        double detXC1 = X[0]    * C[1][1] - X[1]    * C[0][1];
        alpha1 = detXC1 / detC0C1;
        alpha2 = detC0X / detC0C1;
    } else {
        double c0 = C[0][0] + C[0][1];
        double c1 = C[1][0] + C[1][1];
        alpha1 = alpha2 = ((abs(c0) > epsilon) ? X[0] / c0 : ((abs(c1) > epsilon) ? X[1] / c1 : 0));
    }

	double segLength = glm::distance(pt2, pt1);
	double eps = epsilon * segLength;
	glm::vec3 handle1, handle2;
    bool noHandle = false;
    if (alpha1 < eps || alpha2 < eps) {
        alpha1 = alpha2 = segLength / 3;
    } else {
		glm::vec3 line = subtract(pt2, pt1);
        handle1 = normalize(tan1, alpha1);
        handle2 = normalize(tan2, alpha2);
        if (glm::dot(handle1,line) - glm::dot(handle2,line) > segLength * segLength) {
            alpha1 = alpha2 = segLength / 3;
            noHandle = true;
        }
    }
    
    vector<glm::vec3> curve;
    curve.push_back(pt1);
    if (noHandle) {
        curve.push_back(add(pt1, normalize(tan1, alpha1)));
        curve.push_back(add(pt2, normalize(tan2, alpha2)));
    } else {
        curve.push_back(add(pt1, handle1));
        curve.push_back(add(pt2, handle2));
    }
    curve.push_back(pt2);
    return curve;
}

bool ofxPathFitter::reparameterize(int first, int last, vector<double> u, vector<glm::vec3> curve) {
    for (int i = first; i <= last; i++) {
        u[i - first] = findRoot(curve, points[i], u[i - first]);
    }
    for (int i = 1, l = u.size(); i < l; i++) {
        if (u[i] <= u[i - 1])
            return false;
    }
    return true;
}

double ofxPathFitter::findRoot(vector<glm::vec3> curve, glm::vec3 point, double u) {
	vector<glm::vec3> curve1;
	vector<glm::vec3> curve2;
    for (int i = 0; i <= 2; i++) {
        curve1.push_back(multiply(subtract(curve[i + 1], curve[i]), 3));
    }
    for (int i = 0; i <= 1; i++) {
        curve2.push_back(multiply(subtract(curve1[i + 1], curve1[i]), 2));
    }
	glm::vec3 pt = evaluate(3, curve, u);
	glm::vec3 pt1 = evaluate(2, curve1, u);
	glm::vec3 pt2 = evaluate(1, curve2, u);
	glm::vec3 diff = subtract(pt, point);
    double df = glm::dot(pt1,pt1) + glm::dot(diff,pt2);
    return (df > -0.000001 && df < 0.000001) ? u : u - glm::dot(diff,pt1) / df;
}

glm::vec3 ofxPathFitter::evaluate(int degree, vector<glm::vec3> curve, double t) {
    for (int i = 1; i <= degree; i++) {
        for (int j = 0; j <= degree - i; j++) {
			curve[j] = add(multiply(curve[j], 1 - t), multiply(curve[j + 1], t));
        }
    }
    return curve[0];
}

vector<double> ofxPathFitter::chordLengthParameterize(int first, int last) {
    vector<double> u;
    u.push_back(0);
    for (int i = first + 1; i <= last; i++) {
        u.push_back(u[i - first - 1] + glm::distance(points[i], points[i - 1]));
    }
    int m = last - first;
    for (int i = 1; i <= m; i++) {
        u[i] /= u[m];
    }
    return u;
}

PathFitterError ofxPathFitter::findMaxError(int first, int last, vector<glm::vec3> curve, vector<double> u) {
    int index = (last - first + 1) / 2;
    int maxDist = 0;
    for (int i = first + 1; i < last; i++) {
		glm::vec3 P = evaluate(3, curve, u[i - first]);
		glm::vec3 v = subtract(P, points[i]);
        int dist = v.x * v.x + v.y * v.y;
        if (dist >= maxDist) {
            maxDist = dist;
            index = i;
        }
    }
    return PathFitterError(index, maxDist);
}

glm::vec3 ofxPathFitter::add(glm::vec3 p1, glm::vec3 p2) {
	return glm::vec3(p1.x + p2.x, p1.y + p2.y, 0.0f);
}

glm::vec3 ofxPathFitter::subtract(glm::vec3 p1, glm::vec3 p2) {
	return glm::vec3(p1.x - p2.x, p1.y - p2.y, 0.0f);
}

glm::vec3 ofxPathFitter::multiply(glm::vec3 p1, double t) {
	return glm::vec3(p1.x * t, p1.y * t, 0.0f);
}

glm::vec3 ofxPathFitter::normalize(glm::vec3 pt, double length = 1) {
    double current = pt.length();
	double scale = ((current != 0) ? length / current : 0);
	glm::vec3 point = glm::vec3(pt.x * scale, pt.y * scale,0.0f);
    return point;
}
