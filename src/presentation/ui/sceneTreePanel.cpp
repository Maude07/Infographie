#include "presentation/ui/sceneTreePanel.h"

namespace {
    const ofColor rowColor(40, 40, 46);
    const ofColor rowHoverColor(60, 60, 68);
    const ofColor rowSelectedColor(90, 140, 240);
    const ofColor rowTextColor(230, 230, 235);
    const glm::vec2 rowTextOffset(10, 20);
}

void SceneTreePanel::draw(const Scene & scene, const std::unordered_set<SceneObject*> & selection, const glm::vec2 & mouse) const {
    const auto & objects = scene.getObjects();

    ofPushStyle();
    ofFill();
    for (size_t i = 0; i < objects.size(); ++i) {
        ofRectangle row = getRowBounds(i);

        if (selection.count(objects[i].get()) > 0) {
            ofSetColor(rowSelectedColor);
        } else if (row.inside(mouse.x, mouse.y)) {
            ofSetColor(rowHoverColor);
        } else {
            ofSetColor(rowColor);
        }
        ofDrawRectangle(row);

        ofSetColor(rowTextColor);
        ofDrawBitmapString(objects[i]->name, row.getPosition() + glm::vec3(rowTextOffset, 0));
    }
    ofPopStyle();
}

SceneObject * SceneTreePanel::hitTest(const Scene & scene, float x, float y) const {
    const auto & objects = scene.getObjects();
    for (size_t i = 0; i < objects.size(); ++i) {
        if (getRowBounds(i).inside(x, y)) return objects[i].get();
    }
    return nullptr;
}

ofRectangle SceneTreePanel::getRowBounds(size_t index) const {
    float x = ofGetWidth() - rowWidth - marginRight;
    float y = marginTop + index * (rowHeight + rowGap);
    return ofRectangle(x, y, rowWidth, rowHeight);
}
