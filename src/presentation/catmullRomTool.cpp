#include "presentation/catmullRomTool.h"

unique_ptr<SceneObject> CatmullRomTool::commit(const ofColor & lineColor, float lineWidth, const string & name) {
    if (static_cast<int>(points.size()) < SceneShapeCatmullRom::minControlPoints) {
        ofLog() << "<catmull-rom: " << points.size() << "/"
            << SceneShapeCatmullRom::minControlPoints << " points minimum>";
        return nullptr;
    }

    auto shape = make_unique<SceneShapeCatmullRom>();
    shape->name = name;
    shape->filled = false;
    shape->lineColor = lineColor;
    shape->lineWidth = lineWidth;
    shape->setControlPoints(points);
    points.clear();
    return shape;
}

void CatmullRomTool::draw(const glm::vec2 & mouse, const ofColor & lineColor, float lineWidth) const {
    if (points.empty()) return;

    vector<glm::vec2> preview = points;
    preview.push_back(mouse);

    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(lineWidth);
    ofSetColor(lineColor);
    SceneShapeCatmullRom::buildCurve(preview).draw();
    SceneShapeCatmullRom::drawControlPolygon(points);

    ofSetColor(255);
    ofDrawBitmapStringHighlight(ofToString(points.size()) + "/"
        + ofToString(SceneShapeCatmullRom::minControlPoints) + " points   (Entree: terminer)",
        mouse + glm::vec2(12, -12));
    ofPopStyle();
}
