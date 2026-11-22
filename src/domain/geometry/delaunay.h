#pragma once
#include "ofMain.h"
#include <algorithm>

class Delaunay {
public:
	std::vector<std::array<int, 3>> triangulate(const std::vector<glm::vec2> & points);

	static bool isCollinear(const glm::dvec2 & a, const glm::dvec2 & b, const glm::dvec2 & c, double seuil = 0.0001);

	static bool isCollinear(const std::vector<glm::vec2> & points);

	static bool isInCircumcircle(const glm::dvec2 & a, const glm::dvec2 & b, const glm::dvec2 & c, const glm::dvec2 & d);

	static size_t findMaxVertex(const std::vector<glm::vec2> & points);

private:
	struct Triangle {
		std::array<int, 3> v;
		std::array<int, 3> n;
		bool alive = true;
	};

	std::vector<glm::dvec2> vertices;
	std::vector<Triangle> triangles;
	double epsilon = 1e-9;

	static double orient(const glm::dvec2 & a, const glm::dvec2 & b, const glm::dvec2 & c);

	void createBoundingTriangle(size_t v1, int & v2, int & v3);

	bool locate(int p, int & triangle, int & edge) const;

	void splitTriangle(int t, int p);       
	void splitEdge(int t, int edge, int p); 
	void legalizeEdge(int t, int p); 

	int addTriangle(int a, int b, int c, int na, int nb, int nc);
	void replaceNeighbor(int t, int oldNeighbor, int newNeighbor);
	static int indexOfVertex(const Triangle & tri, int vertex);
	static int indexOfNeighbor(const Triangle & tri, int neighbor);
};
