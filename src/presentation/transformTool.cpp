#include "presentation/transformTool.h"

using namespace std;

void TransformTool::select(SceneObject* object, bool additive) {
	isResizing = false;
	isDragging = false;

	if (!object) {
		if (!additive) selection.clear();
		return;
	}

	if (additive) {
		if (selection.erase(object) == 0) selection.insert(object);
	} else {
		selection.clear();
		selection.insert(object);
	}
}

void TransformTool::mousePressed(Scene & scene, float x, float y, bool additive) {
	lastMouse = glm::vec2(x, y);
	isResizing = false;
	isDragging = false;

    if (SceneObject * only = single()) {
		if (getHandleBounds(*only).inside(x, y)) {
			isResizing = true;
			return;
		}
    }

    SceneObject * hit = scene.hitTest(x, y);

	if (!hit) {
		if (!additive) selection.clear();
		return;
	}

	if (additive) {
		select(hit, true);
		isDragging = selection.count(hit) > 0;
	} else {
		if (selection.count(hit) == 0) select(hit);
		isDragging = true;
	}
}

void TransformTool::mouseDragged(float x, float y) {
	glm::vec2 mouse(x, y);

	if (isResizing) {
		if (SceneObject * only = single()) {
			only->size = {
				max(minSize, x - only->position.x),
				max(minSize, y - only->position.y)
			};
		}
	} else if (isDragging) {
		glm::vec2 delta = mouse - lastMouse;
		for (SceneObject* object : selection) {
			object->position += delta;
		}
	}
	lastMouse = mouse;
}

void TransformTool::mouseReleased() {
    isResizing = false;
	isDragging = false;
}

void TransformTool::drawOverlay() const {
    if (selection.empty()) return;

    ofPushStyle();
    ofSetColor(255, 255, 0);
    ofSetLineWidth(1);

    ofNoFill();
	for (const SceneObject * object : selection) {
		ofDrawRectangle(object->getBounds());
	}

	if (const SceneObject* only = single()) {
		ofFill();
		ofDrawRectangle(getHandleBounds(*only));
	}
    ofPopStyle();
}

ofRectangle TransformTool::getHandleBounds(const SceneObject & object) const {
	glm::vec2 corner = object.position + object.size;
    return ofRectangle(
        corner.x - handleSize / 2.0f,
        corner.y - handleSize / 2.0f,
        handleSize, handleSize);
}
