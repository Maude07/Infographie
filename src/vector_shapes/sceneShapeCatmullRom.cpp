#include "vector_shapes/sceneShapeCatmullRom.h"

glm::vec2 SceneShapeCatmullRom::evaluate(const glm::vec2 & p0, const glm::vec2 & p1,
    const glm::vec2 & p2, const glm::vec2 & p3, float t) {
        const float t2 = t * t;
        const float t3 = t2 * t;
        return 0.5f * ((2.0f * p1)
            + (-p0 + p2) * t
            + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2
            + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

ofPolyline SceneShapeCatmullRom::buildCurve(const vector<glm::vec2> & pts) {
    ofPolyline line;
    const int n = static_cast<int>(pts.size());
    if (n < 2) return line;

    for (int i = 0; i < n - 1; ++i) {
        const glm::vec2 & p0 = pts[std::max(i - 1, 0)];
        const glm::vec2 & p1 = pts[i];
        const glm::vec2 & p2 = pts[i + 1];
        const glm::vec2 & p3 = pts[std::min(i + 2, n - 1)];

        for (int s = (i == 0 ? 0 : 1); s <= samplesPerSegment; ++s) {
            const float t = static_cast<float>(s) / samplesPerSegment;
            const glm::vec2 p = evaluate(p0, p1, p2, p3, t);
            line.addVertex(p.x, p.y);
        }
    }
    return line;
}

void SceneShapeCatmullRom::drawControlPolygon(const vector<glm::vec2> & pts) {
    ofPushStyle();
    ofSetLineWidth(1.0f);
    ofSetColor(150);
    for (size_t i = 1; i < pts.size(); ++i) ofDrawLine(pts[i - 1], pts[i]);
    ofFill();
    for (const glm::vec2 & p : pts) ofDrawCircle(p, 4.0f);
    ofPopStyle();
}

void SceneShapeCatmullRom::setControlPoints(const vector<glm::vec2> & worldPoints) {
    if (worldPoints.empty()) return;

    glm::vec2 lo = worldPoints.front(), hi = worldPoints.front();
    for (const glm::vec2 & p : worldPoints) {
        lo = glm::min(lo, p);
        hi = glm::max(hi, p);
    }

    position = lo;
    size = glm::max(hi - lo, glm::vec2(1.0f));
    points.clear();
    for (const glm::vec2 & p : worldPoints) points.push_back(p - lo);
    curve = buildCurve(points);
}

void SceneShapeCatmullRom::draw() const {
    ofPushStyle();
    applyStyle();
    ofPushMatrix();
    ofTranslate(position.x, position.y);
    curve.draw();
    if (showControlPoints) drawControlPolygon(points);
    ofPopMatrix();
    ofPopStyle();
}

void SceneShapeCatmullRom::setBounds(const ofRectangle & bounds) {
    const glm::vec2 oldSize = glm::max(size, glm::vec2(1.0f));
    const glm::vec2 newSize = glm::max(glm::vec2(bounds.width, bounds.height), glm::vec2(1.0f));
    const glm::vec2 scale = newSize / oldSize;

    for (glm::vec2 & p : points) p *= scale;
    position = { bounds.x, bounds.y };
    size = newSize;
    curve = buildCurve(points);
}

bool SceneShapeCatmullRom::contains(float x, float y) const {
    if (curve.size() < 2) return false;
    const glm::vec3 query(x - position.x, y - position.y, 0.0f);
    const float tolerance = std::max(lineWidth, clickPadding);
    return glm::distance(curve.getClosestPoint(query), query) <= tolerance;
}
