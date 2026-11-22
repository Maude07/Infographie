#pragma once
#include "vector_shapes/sceneShapeCatmullRom.h"

class CatmullRomTool {
public :
    void addPoint(const glm::vec2 & point) { points.push_back(point); }
    void removeLastPoint() { if (!points.empty()) points.pop_back(); }
    void cancel() { points.clear(); }
    bool isActive() const { return !points.empty(); }

    unique_ptr<SceneObject> commit(const ofColor & lineColor, float lineWidth, const string & name);
    void draw(const glm::vec2 & mouse, const ofColor & lineColor, float lineWidth) const;

private :
    vector<glm::vec2> points;
};
