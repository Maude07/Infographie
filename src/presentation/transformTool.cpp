#include "presentation/transformTool.h"
#include "domain/commands/transformCommand.h"
#include "domain/commands/compositeCommand.h"

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

void TransformTool::captureDragStart() {
	dragStartBounds.clear();
	for (SceneObject * object : selection) {
		dragStartBounds[object] = object->getBounds();
	}
}

void TransformTool::mousePressed(Scene & scene, float x, float y, bool additive) {
	lastMouse = glm::vec2(x, y);
	isResizing = false;
	isDragging = false;
	isInZone = false;

	if (SceneObject * only = single()) {
		if (only->isResizable() && getHandleBounds(*only).inside(x, y)) {
			isResizing = true;
			captureDragStart();
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

	if (isDragging) captureDragStart();
}

void TransformTool::mouseDragged(float x, float y) {
	glm::vec2 mouse(x, y);

	if (isResizing) {
		if (SceneObject * only = single()) {
			ofRectangle b = only->getBounds();
			glm::vec2 p = only->toLocal(mouse);
			float newW = max(minSize, p.x - b.x);
			float newH = max(minSize, p.y - b.y);
			only->setBounds(ofRectangle(b.x, b.y, newW, newH));
		}
	} else if (isDragging) {
		for (SceneObject* object : selection) {
			object->position += object->toLocal(mouse) - object->toLocal(lastMouse);
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
			if (zone.intersects(object->getScreenBounds())) {
				selection.insert(object.get());
			}
		}
		isInZone = false;
	}

	if ((isResizing || isDragging) && history && !dragStartBounds.empty()) {
		vector<unique_ptr<Command>> changes;

		for (auto & [object, startBounds] : dragStartBounds) {
			ofRectangle currentBounds = object->getBounds();

			if (currentBounds.x != startBounds.x || currentBounds.y != startBounds.y || currentBounds.width != startBounds.width || currentBounds.height != startBounds.height) {

				changes.push_back(make_unique<TransformCommand>(object, startBounds, currentBounds));
			}
		}

		if (!changes.empty()) {
			if (changes.size() == 1) {
				history->push(move(changes.front()));
			} else {
				history->push(make_unique<CompositeCommand>(move(changes)));
			}
		}
	}
	dragStartBounds.clear();
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
			ofDrawRectangle(object->getScreenBounds());
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
	ofRectangle bounds = object.getScreenBounds();
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
