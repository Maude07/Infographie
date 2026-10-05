#include "presentation/transformTool.h"

using namespace std;

void TransformTool::select(SceneObject* object, bool additive) {
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

	isResizing = false;
	isDragging = false;
	isInZone = false;
}

void TransformTool::mousePressed(Scene & scene, float x, float y, bool additive) {
	lastMouse = glm::vec2(x, y);
	isResizing = false;
	isDragging = false;
	isInZone = false;

	if (SceneObject * only = single()) {
		if (only->isResizable() && getHandleBounds(*only).inside(x, y)) {
			isResizing = true;
			return;
		}
	}

	SceneObject * hit = scene.hitTest(x, y);

	if (!hit) {
		if (!additive) selection.clear();
		isInZone = true;
		zoneStart = zoneEnd = glm::vec2(x, y);
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
			ofRectangle b = only->getBounds();
			float newW = max(minSize, x - b.x);
			float newH = max(minSize, y - b.y);
			only->setBounds(ofRectangle(b.x, b.y, newW, newH));
		}
	} else if (isDragging) {
		glm::vec2 delta = mouse - lastMouse;
		for (SceneObject* object : selection) {
			object->position += delta;
		}
	} else if (isInZone) {
		zoneEnd = glm::vec2(x, y);
	}
	lastMouse = mouse;
}

void TransformTool::mouseReleased(Scene & scene) {
	if (isInZone) {
		ofRectangle zone = getZoneBounds();
		for (const auto& object : scene.getObjects()) {
			if (zone.intersects(object->getBounds())) {
				selection.insert(object.get());
			}
		}
		isInZone = false;
	}
    isResizing = false;
	isDragging = false;
}

void TransformTool::drawOverlay() const {
	ofPushStyle();

	if (isInZone) {
		ofRectangle zone = getZoneBounds();
		ofEnableAlphaBlending();
		ofFill();
		ofSetColor(255, 255, 0, 50);
		ofSetLineWidth(1);
		ofDrawRectangle(zone);

		ofNoFill();
		ofSetColor(255, 255, 0, 100);
		ofSetLineWidth(1);
		ofDrawRectangle(zone);
	}

	if (!selection.empty()) {
		ofSetColor(255, 255, 0);
		ofSetLineWidth(1);
		ofNoFill();
		for (const SceneObject * object : selection) {
			ofDrawRectangle(object->getBounds());
		}
		if (const SceneObject * only = single()) {
			if (only->isResizable()) {
				ofFill();
				ofDrawRectangle(getHandleBounds(*only));
			}
		}
	}

    ofPopStyle();
}

ofRectangle TransformTool::getHandleBounds(const SceneObject & object) const {
	ofRectangle bounds = object.getBounds();
    return ofRectangle(
        bounds.getRight() - handleSize / 2.0f,
        bounds.getBottom() - handleSize / 2.0f,
        handleSize, handleSize);
}

ofRectangle TransformTool::getZoneBounds() const {
	glm::vec2 topLeft = glm::min(zoneStart, zoneEnd);
	glm::vec2 size = glm::abs(zoneEnd - zoneStart);
	return ofRectangle(topLeft.x, topLeft.y, size.x, size.y);
}
