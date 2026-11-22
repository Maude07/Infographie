#include "delaunay.h"
#include <random>

std::vector<std::array<int, 3>> Delaunay::triangulate(const std::vector<glm::vec2> & points) {
	vertices.clear();
	triangles.clear();

	if (isCollinear(points)) {
		return {};
	}

	const int n = static_cast<int>(points.size());
	vertices.reserve(n + 2);
	for (const glm::vec2 & p : points) {
		vertices.emplace_back(p.x, p.y);
	}

	double minX = vertices[0].x, maxX = vertices[0].x, minY = vertices[0].y, maxY = vertices[0].y;
	for (const glm::dvec2 & p : vertices) {
		minX = std::min(minX, p.x); maxX = std::max(maxX, p.x);
		minY = std::min(minY, p.y); maxY = std::max(maxY, p.y);
	}
	epsilon = 1e-9 * std::max(maxX - minX, maxY - minY);

	size_t v1 = findMaxVertex(points);
	int v2, v3;
	createBoundingTriangle(v1, v2, v3);

	if (orient(vertices[v1], vertices[v2], vertices[v3]) > 0) {
		addTriangle(static_cast<int>(v1), v2, v3, -1, -1, -1);
	} else {
		addTriangle(static_cast<int>(v1), v3, v2, -1, -1, -1);
	}

	std::vector<int> order;
	order.reserve(n - 1);
	for (int i = 0; i < n; ++i) {
		if (i != static_cast<int>(v1)) {
			order.push_back(i);
		}
	}
	std::mt19937 rng(std::random_device {}());
	std::shuffle(order.begin(), order.end(), rng);

	for (int v : order) {
		int t1, edge;
		if (!locate(v, t1, edge)) {
			continue;
		}
		if (edge == -1) {
			splitTriangle(t1, v);
		} else {
			splitEdge(t1, edge, v);
		}
	}

	std::vector<std::array<int, 3>> result;
	for (const Triangle & t : triangles) {
		if (t.alive && t.v[0] < n && t.v[1] < n && t.v[2] < n) {
			result.push_back(t.v);
		}
	}
	return result;
}

bool Delaunay::isCollinear(const glm::dvec2 & a, const glm::dvec2 & b, const glm::dvec2 & c, double seuil) {
	double condition = (a.y - b.y) * (a.x - c.x) - (a.y - c.y) * (a.x - b.x);
	return std::abs(condition) <= seuil;
}

bool Delaunay::isCollinear(const std::vector<glm::vec2> & points) {
	if (points.size() < 3) {
		return true;
	}

	const glm::vec2 & a = points[0];

	size_t bIndex = 0;
	for (size_t i = 1; i < points.size(); ++i) {
		if (glm::distance(a, points[i]) > 1e-6f) {
			bIndex = i;
			break;
		}
	}
	if (bIndex == 0) {
		return true;
	}
	const glm::vec2 & b = points[bIndex];

	for (size_t i = 1; i < points.size(); ++i) {
		glm::vec2 ab = b - a;
		glm::vec2 ac = points[i] - a;
		double cross = (double)ab.x * ac.y - (double)ab.y * ac.x;
		double scale = (double)glm::length(ab) * glm::length(ac);
		if (scale > 1e-12 && std::abs(cross) / scale > 1e-6) {
			return false;
		}
	}
	return true;
}

bool Delaunay::isInCircumcircle(const glm::dvec2 & a, const glm::dvec2 & b, const glm::dvec2 & c, const glm::dvec2 & d) {
	double adx = a.x - d.x, ady = a.y - d.y;
	double bdx = b.x - d.x, bdy = b.y - d.y;
	double cdx = c.x - d.x, cdy = c.y - d.y;

	double ad = adx * adx + ady * ady;
	double bd = bdx * bdx + bdy * bdy;
	double cd = cdx * cdx + cdy * cdy;

	double det = adx * (bdy * cd - bd * cdy)
		- ady * (bdx * cd - bd * cdx)
		+ ad * (bdx * cdy - bdy * cdx);

	double bound = std::abs(adx) * (std::abs(bdy * cd) + std::abs(bd * cdy))
		+ std::abs(ady) * (std::abs(bdx * cd) + std::abs(bd * cdx))
		+ ad * (std::abs(bdx * cdy) + std::abs(bdy * cdx));

	return det > 1e-12 * bound;
}

size_t Delaunay::findMaxVertex(const std::vector<glm::vec2> & points) {
	auto it = std::max_element(points.begin(), points.end(),
		[](const glm::vec2 & p, const glm::vec2 & q) {
			if (p.y != q.y) return p.y < q.y;
			return p.x < q.x;
		});

	return static_cast<size_t>(std::distance(points.begin(), it));
}

double Delaunay::orient(const glm::dvec2 & a, const glm::dvec2 & b, const glm::dvec2 & c) {
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void Delaunay::createBoundingTriangle(size_t v1, int & v2, int & v3) {
	const glm::dvec2 top = vertices[v1];

	double minX = top.x, maxX = top.x, minY = top.y;
	for (const glm::dvec2 & p : vertices) {
		minX = std::min(minX, p.x);
		maxX = std::max(maxX, p.x);
		minY = std::min(minY, p.y);
	}
	double width = maxX - minX;
	double height = top.y - minY;

	double slopeRight = 1.0;
	for (const glm::dvec2 & p : vertices) {
		if (p.x > top.x) {
			slopeRight = std::min(slopeRight, (top.y - p.y) / (p.x - top.x));
		}
	}
	double s2 = slopeRight * 0.5;
	double s3 = s2 * 0.5;

	double M = 2.0 * (4.0 * height / s2 + 3.0 * width) + std::max(width, height);

	v2 = static_cast<int>(vertices.size());
	vertices.emplace_back(top.x + M, top.y - M * s2);
	v3 = static_cast<int>(vertices.size());
	vertices.emplace_back(top.x - M, top.y + M * s3);
}

bool Delaunay::locate(int p, int & triangle, int & edge) const {
	const glm::dvec2 & point = vertices[p];

	for (int t = 0; t < static_cast<int>(triangles.size()); ++t) {
		const Triangle & tri = triangles[t];
		if (!tri.alive) {
			continue;
		}

		int onEdge = -1;
		bool inside = true;
		for (int i = 0; i < 3; ++i) {
			const glm::dvec2 & a = vertices[tri.v[(i + 1) % 3]];
			const glm::dvec2 & b = vertices[tri.v[(i + 2) % 3]];

			if (glm::length(point - a) <= epsilon) {
				return false;
			}

			if (isCollinear(a, b, point, epsilon * glm::length(b - a))) {
				onEdge = i;
			} else if (orient(a, b, point) < 0) {
				inside = false;
				break;
			}
		}

		if (inside) {
			triangle = t;
			edge = onEdge;
			return true;
		}
	}
	return false;
}

void Delaunay::splitTriangle(int t, int p) {
	Triangle old = triangles[t];
	triangles[t].alive = false;

	int a = old.v[0], b = old.v[1], c = old.v[2];
	int nBC = old.n[0];
	int nCA = old.n[1];
	int nAB = old.n[2];

	int base = static_cast<int>(triangles.size());
	int tAB = base, tBC = base + 1, tCA = base + 2;

	addTriangle(a, b, p, tBC, tCA, nAB);
	addTriangle(b, c, p, tCA, tAB, nBC);
	addTriangle(c, a, p, tAB, tBC, nCA);

	replaceNeighbor(nAB, t, tAB);
	replaceNeighbor(nBC, t, tBC);
	replaceNeighbor(nCA, t, tCA);

	legalizeEdge(tAB, p);
	legalizeEdge(tBC, p);
	legalizeEdge(tCA, p);
}

void Delaunay::splitEdge(int t, int edge, int p) {
	Triangle t1 = triangles[t];
	int k = edge;
	int c = t1.v[k];
	int a = t1.v[(k + 1) % 3];
	int b = t1.v[(k + 2) % 3];
	int nCA = t1.n[(k + 2) % 3];
	int nBC = t1.n[(k + 1) % 3];

	int t2Index = t1.n[k];
	if (t2Index == -1) {
		triangles[t].alive = false;
		int base = static_cast<int>(triangles.size());
		int tA = base, tB = base + 1;
		addTriangle(c, a, p, -1, tB, nCA);
		addTriangle(c, p, b, -1, nBC, tA);
		replaceNeighbor(nCA, t, tA);
		replaceNeighbor(nBC, t, tB);
		legalizeEdge(tA, p);
		legalizeEdge(tB, p);
		return;
	}

	Triangle t2 = triangles[t2Index];
	int j = indexOfNeighbor(t2, t);
	int d = t2.v[j];
	int nDB = t2.n[(j + 2) % 3];
	int nAD = t2.n[(j + 1) % 3];

	triangles[t].alive = false;
	triangles[t2Index].alive = false;

	int base = static_cast<int>(triangles.size());
	int tA = base, tB = base + 1, tC = base + 2, tD = base + 3;

	addTriangle(c, a, p, tD, tB, nCA);
	addTriangle(c, p, b, tC, nBC, tA);
	addTriangle(d, b, p, tB, tD, nDB);
	addTriangle(d, p, a, tA, nAD, tC);

	replaceNeighbor(nCA, t, tA);
	replaceNeighbor(nBC, t, tB);
	replaceNeighbor(nDB, t2Index, tC);
	replaceNeighbor(nAD, t2Index, tD);

	legalizeEdge(tA, p);
	legalizeEdge(tB, p);
	legalizeEdge(tC, p);
	legalizeEdge(tD, p);
}

void Delaunay::legalizeEdge(int t, int p) {
	const Triangle tri = triangles[t];
	int i = indexOfVertex(tri, p);
	int nb = tri.n[i];
	if (nb == -1) {
		return;
	}

	const Triangle other = triangles[nb];
	int j = indexOfNeighbor(other, t);
	int q = tri.v[(i + 1) % 3];
	int r = tri.v[(i + 2) % 3];
	int d = other.v[j];

	if (!isInCircumcircle(vertices[p], vertices[q], vertices[r], vertices[d])) {
		return;
	}

	int nPQ = tri.n[(i + 2) % 3];
	int nRP = tri.n[(i + 1) % 3];
	int nQD = other.n[(j + 1) % 3];
	int nDR = other.n[(j + 2) % 3];

	triangles[t].v = { p, q, d };
	triangles[t].n = { nQD, nb, nPQ };
	triangles[nb].v = { p, d, r };
	triangles[nb].n = { nDR, nRP, t };

	replaceNeighbor(nQD, nb, t);
	replaceNeighbor(nRP, t, nb);

	legalizeEdge(t, p);
	legalizeEdge(nb, p);
}

int Delaunay::addTriangle(int a, int b, int c, int na, int nb, int nc) {
	Triangle tri;
	tri.v = { a, b, c };
	tri.n = { na, nb, nc };
	triangles.push_back(tri);
	return static_cast<int>(triangles.size()) - 1;
}

void Delaunay::replaceNeighbor(int t, int oldNeighbor, int newNeighbor) {
	if (t == -1) {
		return;
	}
	for (int & n : triangles[t].n) {
		if (n == oldNeighbor) {
			n = newNeighbor;
			return;
		}
	}
}

int Delaunay::indexOfVertex(const Triangle & tri, int vertex) {
	for (int i = 0; i < 3; ++i) {
		if (tri.v[i] == vertex) return i;
	}
	return -1;
}

int Delaunay::indexOfNeighbor(const Triangle & tri, int neighbor) {
	for (int i = 0; i < 3; ++i) {
		if (tri.n[i] == neighbor) return i;
	}
	return -1;
}
