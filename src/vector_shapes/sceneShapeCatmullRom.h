#pragma once
#include "domain/shapes/2d/scenePrimitive.h"

class SceneShapeCatmullRom : public ScenePrimitive {
public:
    static constexpr int minControlPoints = 5;
    static constexpr int samplesPerSegment = 20;
    static constexpr float clickPadding = 10.0f;

    bool showControlPoints = true;

    void setControlPoints(const vector<glm::vec2> & worldPoints);

    static glm::vec2 evaluate(const glm::vec2 & p0, const glm::vec2 & p1,
        const glm::vec2 & p2, const glm::vec2 & p3, float t);
    static ofPolyline buildCurve(const vector<glm::vec2> & points);
    static void drawControlPolygon(const vector<glm::vec2> & points);

    void draw() const override;
    void setBounds(const ofRectangle & bounds) override;
    bool contains(float x, float y) const override;

private:
    vector<glm::vec2> points;
    ofPolyline curve;
};
