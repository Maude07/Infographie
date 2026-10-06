#include "presentation/transformTool.h"
#include "domain/commands/transformCommand.h"
#include "domain/commands/compositeCommand.h"

using namespace std;

namespace {
	ofRectangle visualBounds(const SceneObject & object) {
		ofRectangle bounds = object.getScreenBounds();
		glm::vec2 center = bounds.getCenter();
		ofRectangle result(center.x, center.y, 0, 0);
		for (const glm::vec3 & corner : { bounds.getTopLeft(), bounds.getTopRight(), bounds.getBottomRight(), bounds.getBottomLeft() }) {
			glm::vec2 rotated = center + rotateVector(glm::vec2(corner) - center, object.rotation);
			result.growToInclude(rotated.x, rotated.y);
		}
		return result;
	}

	float shiftInside(float low, float high, float min, float max) {
		float lowest = min - low;
		float highest = max - high;
		if (lowest > highest) return (lowest + highest) / 2.0f;
		return ofClamp(0.0f, lowest, highest);
	}
}

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
	isRotating = false;
}

void TransformTool::captureDragStart() {
	dragStartStates.clear();
	dragStartPositions.clear();
	dragStartMouse = lastMouse;
	for (SceneObject * object : selection) {
		dragStartStates[object] = TransformState::of(*object);
		dragStartPositions[object] = object->position;
	}
	dragStartArea = selectionArea();
}

void TransformTool::moveSelection(const glm::vec2 & target) {
	for (SceneObject * object : selection) {
		object->position = dragStartPositions[object] + object->toLocal(target) - object->toLocal(dragStartMouse);
	}
}

ofRectangle TransformTool::selectionArea() const {
	ofRectangle area;
	bool first = true;
	for (const SceneObject * object : selection) {
		if (first) {
			area = visualBounds(*object);
			first = false;
		} else {
			area.growToInclude(visualBounds(*object));
		}
	}
	return area;
}

void TransformTool::fitInCanvas(SceneObject & object, bool shrinkWidth, bool shrinkHeight) const {
	if (canvas.isEmpty()) return;

	auto fits = [&] {
		ofRectangle area = visualBounds(object);
		return area.width <= canvas.width + 0.5f && area.height <= canvas.height + 0.5f;
	};

	if (!fits() && object.isResizable()) {
		if (!shrinkWidth && !shrinkHeight) {
			shrinkWidth = true;
			shrinkHeight = true;
		}

		ofRectangle requested = object.getBounds();
		glm::vec2 center = requested.getCenter();
		auto resize = [&](float factor) {
			float width = shrinkWidth ? max(SceneObject::minSize, requested.width * factor) : requested.width;
			float height = shrinkHeight ? max(SceneObject::minSize, requested.height * factor) : requested.height;
			ofRectangle bounds;
			bounds.setFromCenter(center.x, center.y, width, height);
			object.setBounds(bounds);
		};

		float low = 0.0f;
		float high = 1.0f;
		for (int i = 0; i < 12; ++i) {
			float middle = (low + high) / 2.0f;
			resize(middle);
			if (fits()) {
				low = middle;
			} else {
				high = middle;
			}
		}
		resize(low);
	}

	keepInCanvas(object);
}

void TransformTool::keepInCanvas(SceneObject & object) const {
	if (canvas.isEmpty()) return;

	for (int i = 0; i < 3; ++i) {
		ofRectangle area = visualBounds(object);
		glm::vec2 shift(
			shiftInside(area.getLeft(), area.getRight(), canvas.getLeft(), canvas.getRight()),
			shiftInside(area.getTop(), area.getBottom(), canvas.getTop(), canvas.getBottom()));
		if (glm::length(shift) < 0.5f) return;

		glm::vec2 center = area.getCenter();
		object.position += object.toLocal(center + shift) - object.toLocal(center);
	}
}


void TransformTool::mousePressed(Scene & scene, float x, float y, bool additive) {
	lastMouse = glm::vec2(x, y);
	isResizing = false;
	isDragging = false;
	isInZone = false;
	isRotating = false;

	if (SceneObject * only = single()) {
		glm::vec2 local = only->toLocal(x, y);

		if (only->isResizable() && getHandleBounds(*only).inside(local.x, local.y)) {
			isResizing = true;
			ofRectangle bounds = only->getBounds();
			resizeAnchor = only->toWorld({ bounds.x, bounds.y });
			glm::vec2 grab = rotateVector(only->toLocal(glm::vec2(x, y)) - resizeAnchor, -only->rotation);
			resizeGrabOffset = grab - glm::vec2(bounds.width, bounds.height);
			aspectRatio = bounds.height > 0.0f ? bounds.width / bounds.height : 1.0f;
			captureDragStart();
			return;
		}

		if (only->isRotatable() && getRotateHandle(*only).inside(local.x, local.y)) {
			isRotating = true;
			rotateStartAngle = angleFromCenter(*only, only->toLocal(lastMouse));
			rotateStartValue = only->rotation;
			captureDragStart();
			return;
		}

		if (only->isRotatable() && getRotateHandle(*only).inside(local.x, local.y)) {
			isRotating = true;
			rotateStartAngle = angleFromCenter(*only, lastMouse);
			rotateStartValue = only->rotation;
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
		if (SceneObject * only = single()) resizeTo(*only, only->toLocal(mouse)); 
	} else if (isRotating) {
		if (SceneObject * only = single()) rotateTo(*only, only->toLocal(mouse));
	} else if (isDragging) {
		glm::vec2 delta = mouse - dragStartMouse;
		if (!canvas.isEmpty()) {
			delta.x = ofClamp(delta.x, min(0.0f, canvas.getLeft() - dragStartArea.getLeft()), max(0.0f, canvas.getRight() - dragStartArea.getRight()));
			delta.y = ofClamp(delta.y, min(0.0f, canvas.getTop() - dragStartArea.getTop()), max (0.0f, canvas.getBottom() - dragStartArea.getBottom()));
		}
		glm::vec2 target = dragStartMouse + delta;
		moveSelection(target);

		if (!canvas.isEmpty()) {
			for (int i = 0; i < 2; ++i) {
				ofRectangle area = selectionArea();
				glm::vec2 excess(
					max(0.0f, area.getRight() - max(canvas.getRight(), dragStartArea.getRight()))
						+ min (0.0f, area.getLeft() - min(canvas.getLeft(), dragStartArea.getLeft())),
					max(0.0f, area.getBottom() - max(canvas.getBottom(), dragStartArea.getBottom()))
						+ min(0.0f, area.getTop() - min(canvas.getTop(), dragStartArea.getTop())));
				if (glm::length(excess) < 0.5f) break;
				target -= excess;
				moveSelection(target);
			}
		}
	} else if (isInZone) {
		zoneEnd = mouse;
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

	if ((isResizing || isDragging || isRotating) && history && !dragStartStates.empty()) {
		vector<unique_ptr<Command>> changes;

		for (auto & [object, startState] : dragStartStates) {
			TransformState currentState = TransformState::of(*object);
			if (currentState != startState) {
				changes.push_back(make_unique<TransformCommand>(object, startState, currentState));
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

	dragStartStates.clear();
	isResizing = false;
	isDragging = false;
	isRotating = false;
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
		for (const SceneObject * object : selection) {
			ofRectangle bounds = object->getScreenBounds();

			ofPushMatrix();
			ofTranslate(bounds.getCenter());
			ofRotateDeg(object->rotation);
			ofTranslate(-bounds.getCenter());

			ofNoFill();
			ofDrawRectangle(bounds);

			if (object == single()) {
				if (object->isResizable()) {
					ofFill();
					ofDrawRectangle(getHandleBounds(*object));
				}
				if (object->isRotatable()) {
					glm::vec2 handle = getRotateHandle(*object).getCenter();
					ofDrawLine(bounds.getCenter().x, bounds.getTop(), handle.x, handle.y + handleSize / 2.0f);
					ofFill();
					ofDrawCircle(handle, handleSize / 2.0f);
				}
			}
			ofPopMatrix();
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

void TransformTool::resizeTo(SceneObject & object, const glm::vec2 & mouse) {
	glm::vec2 local = rotateVector(mouse - resizeAnchor, -object.rotation) - resizeGrabOffset;
	float width = max(minSize, local.x);
	float height = max(minSize, local.y);

	if (ofGetKeyPressed(OF_KEY_SHIFT)) {
		if (width / height > aspectRatio) height = width / aspectRatio;
		else width = height * aspectRatio;
	}

	glm::vec2 center = resizeAnchor + rotateVector(glm::vec2(width, height) * 0.5f, object.rotation);
	ofRectangle bounds;
	bounds.setFromCenter(center.x, center.y, width, height);
	object.setBounds(bounds);
}

void TransformTool::rotateTo(SceneObject & object, const glm::vec2 & mouse) {
	float angle = rotateStartValue + angleFromCenter(object, mouse) - rotateStartAngle;
	if (ofGetKeyPressed(OF_KEY_SHIFT)) {
		angle = round(angle / rotationSnap) * rotationSnap;
	}
	object.rotation = ofWrapDegrees(angle);
}

float TransformTool::angleFromCenter(const SceneObject & object, const glm::vec2 & point) const {
	glm::vec2 fromCenter = point - object.getCenter();
	return glm::degrees(atan2(fromCenter.y, fromCenter.x));
}

ofRectangle TransformTool::getRotateHandle(const SceneObject & object) const {
	ofRectangle bounds = object.getScreenBounds();
	ofRectangle handle;
	handle.setFromCenter(bounds.getCenter().x, bounds.getTop() - rotateHandleDistance, handleSize, handleSize);
	return handle;
}

