#include "transformTool.h"

void TransformTool::mousePressed(Scene & scene, float x, float y) {
    if (selection && getHandleBounds().inside(x, y)) {
        isResizing = true;
        return;
    }

    isResizing = false;
    selection = scene.hitTest(x, y);

    if (selection) {
        dragOffset = glm::vec2(x, y) - selection->position;
    }
}

void TransformTool::mouseDragged(float x, float y) {
    if (!selection) return;

    if (isResizing) {
        selection->size = {
            std::max(minSize, x - selection->position.x),
            std::max(minSize, y - selection->position.y)
        };
    } else {
        selection->position = glm::vec2(x, y) - drag_offset;
    }
}

void TransformTool::mouseReleased() {
    isResizing = false;
}

void TransformTool::DrawOverlay() const {
    if (!selection) return;

    ofPushStyle();
    ofSetColor(255, 255, 0);
    ofSetLineWidth(1);

    ofNoFill();
    ofDrawRectangle(selection->getBounds());

    ofFill();
    ofDrawRectangle(getHandleBounds());

    ofPopStyle();
}

ofRectangle TransformTool::getHandleBounds() const {
    glm::vec2 corner = selection->position + selection->size;
    return ofRectangle(
        corner.x - handleSize / 2.0f,
        corner.y - handleSize / 2.0f,
        handleSize, handleSize);
}
