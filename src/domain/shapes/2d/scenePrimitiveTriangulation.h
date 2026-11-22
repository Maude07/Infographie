#pragma once
#include "domain/shapes/2d/scenePrimitive.h"
#include "ofMain.h"

class ScenePrimitiveTriangulation : public ScenePrimitive {
public:
	bool build(const std::vector<glm::vec2> & points);

	void draw() const override;
	ofRectangle getBounds() const override;
	void setBounds(const ofRectangle & bounds) override;
	bool contains(float x, float y) const override;

	size_t vertexCount() const { return vertices.size(); }
	size_t triangleCount() const { return triangles.size(); }

private:
	std::vector<glm::vec2> vertices;
	std::vector<std::array<int, 3>> triangles;
	ofMesh mesh;

	void rebuildMesh();
	void syncBounds();
};
