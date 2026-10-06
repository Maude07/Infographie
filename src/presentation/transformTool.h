#pragma once

#include "domain/scene/sceneObject.h"
#include "domain/scene/scene.h"
#include "domain/commands/historyManager.h"
#include <unordered_map>

using namespace std;

class TransformTool {

public: 
    void mousePressed(Scene & scene, float x, float y, bool additive);
    void mouseDragged(float x, float y);
    void mouseReleased(Scene & scene);
    void drawOverlay() const;

    const unordered_set<SceneObject *> & getSelection() const { return selection; }
	bool hasSelection() const { return !selection.empty(); }
	void select(SceneObject * object, bool additive = false);
    void clearSelection() {
		selection.clear();
		isResizing = false;
		isDragging = false;
		isInZone = false;
        isRotating = false;
	}
    bool isOverHandle(float x, float y) const {
		if (selection.size() != 1) return false;
		SceneObject * object = *selection.begin();
        glm::vec2 local = object->toLocal(x, y);
		
        return object && object->isResizable() 
        && getHandleBounds(*object).inside(local.x, local.y);
    } 
    bool isResizingSelection() const { return isResizing; }

	void setHistory(HistoryManager * history) { this->history = history; }


private:
	unordered_set<SceneObject *> selection;
    glm::vec2 dragOffset;
	glm::vec2 lastMouse;
	glm::vec2 zoneStart;
	glm::vec2 zoneEnd;
    glm::vec2 resizeAnchor;
    bool isResizing = false;
	bool isDragging = false;
	bool isInZone = false;
    bool isRotating = false;
    float minSize = 20.0f;
    float aspectRatio = 1.0f;
    float rotateStartAngle = 0.0f;
    float rotateStartValue = 0.0f;
    static constexpr float handleSize = 10.0f;
    static constexpr float rotateHandleDistance = 25.0f;
    static constexpr float rotationSnap = 15.0f;

	HistoryManager * history = nullptr;
	unordered_map<SceneObject *, ofRectangle> dragStartBounds;
	void captureDragStart();

	SceneObject * single() const { return selection.size() == 1 ? *selection.begin() : nullptr; }
	ofRectangle getHandleBounds(const SceneObject & object) const;
	ofRectangle getZoneBounds() const;
    ofRectangle getRotateHandle(const SceneObject & object) const;

    float angleFromCenter(const SceneObject & object, const glm::vec2 & point) const;
    void resizeTo(SceneObject & object, const glm::vec2 & mouse);
    void rotateTo(SceneObject & object, const glm::vec2 & mouse);
};
