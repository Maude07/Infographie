#include "presentation/transformTool.h"

using namespace std;

void TransformTool::mousePressed(Scene & scene, float x, float y) {
    glm::vec2 mouse(x, y);

    if (selection) {
        glm::vec2 local = selection->toLocal(x, y);

        if(selection->isResizable() && getResizeHandle().inside(local.x, local.y)) {
            mode = Mode::Resize;
            ofRectangle bounds = selection->getBounds();
            resizeAnchor = selection->toWorld({ bounds.x, bounds.y });
            aspectRatio = bounds.height > 0.0f ? bounds.width / bounds.height : 1.0f;
            return;
        }

        if(selection->isRotatable() && getRotateHandle().inside(local.x, local.y)) {
            mode = Mode::Rotate;
            rotateStartAngle = angleFromCenter(mouse);
            rotateStartValue = selection->rotation;
            return;
        }
    }

    selection = scene.hitTest(x, y);
    mode = selection ? Mode::Move : Mode::None;

    if (selection) {
        dragOffset = mouse - selection->getCenter();
    }
}

void TransformTool::mouseDragged(float x, float y) {
    if (!selection) return;
    glm::vec2 mouse(x, y);

    switch (mode) {
        case Mode::Move: {
            ofRectangle bounds = selection->getBounds();
            glm::vec2 center = mouse - dragOffset;
            bounds.setFromCenter(center.x, center.y, bounds.width, bounds.height);
            selection->setBounds(bounds);
            break;
        }
        case Mode::Resize:
            resizeTo(mouse);
            break;
        case Mode::Rotate:
            rotateTo(mouse);
            break;
        case Mode::None:
            break;
    }
}

void TransformTool::mouseReleased() {
    mode = Mode::None;
}

void TransformTool::resizeTo(const glm::vec2 & mouse) {
    glm::vec2 local = rotateVector(mouse - resizeAnchor, -selection->rotation);
    float width = max(SceneObject::minSize, local.x);
    float height = max(SceneObject::minSize, local.y);

    if (ofGetKeyPressed(OF_KEY_SHIFT)) {
        if (width / height > aspectRatio) height = width / aspectRatio;
        else width = height * aspectRatio;
    }

    glm::vec2 center = resizeAnchor + rotateVector(glm::vec2(width, height) * 0.5f, selection->rotation);
    ofRectangle bounds;
    bounds.setFromCenter(center.x, center.y, width, height);
    selection->setBounds(bounds);
}

void TransformTool::rotateTo(const glm::vec2 & mouse) {
    float angle = rotateStartValue + angleFromCenter(mouse) - rotateStartAngle;

    if (ofGetKeyPressed(OF_KEY_SHIFT)) {
        angle = round(angle / rotationSnap) * rotationSnap;
    }
    selection->rotation = ofWrapDegrees(angle);
}

float TransformTool::angleFromCenter(const glm::vec2 & point) const {
    glm::vec2 fromCenter = point - selection->getCenter();
    return glm::degrees(atan2(fromCenter.y, fromCenter.x));
}

void TransformTool::drawOverlay() const {
    if (!selection) return;

    ofPushStyle();
    ofPushMatrix();
    selection->applyTransform();

    ofSetColor(255, 255, 0);
    ofSetLineWidth(1);

    ofRectangle bounds = selection->getBounds();
    ofNoFill();
    ofDrawRectangle(bounds);

    if (selection->isResizable()) {
        ofFill();
        ofDrawRectangle(getResizeHandle());
    }

    if (selection->isRotatable()) {
        glm::vec2 handle = getRotateHandle().getCenter();
        ofDrawLine(bounds.getCenter().x, bounds.getTop(), handle.x, handle.y + handleSize / 2.0f);
        ofFill();
        ofDrawCircle(handle, handleSize / 2.0f);
    }

    ofPopMatrix();
    ofPopStyle();
}

ofRectangle TransformTool::getResizeHandle() const { 
    ofRectangle bounds = selection->getBounds();
    return ofRectangle(
        bounds.getRight() - handleSize / 2.0f,
        bounds.getBottom() - handleSize / 2.0f,
        handleSize, handleSize);
}

ofRectangle TransformTool::getRotateHandle() const {
    ofRectangle bounds = selection->getBounds();
    ofRectangle handle;
    handle.setFromCenter(bounds.getCenter().x, bounds.getTop() - rotateHandleDistance, handleSize, handleSize);
    return handle;
}

