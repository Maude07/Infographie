#pragma once

#include "domain/sceneObject.h"
#include "domain/scene.h"

class TransformTool {

public: 
    void mousePressed(Scene & scene, float x, float y, bool additive);
    void mouseDragged(float x, float y);
    void mouseReleased(Scene & scene);
    void drawOverlay() const;

    const std::unordered_set<SceneObject *> & getSelection() const { return selection; }
	bool hasSelection() const { return !selection.empty(); }
	void select(SceneObject * object, bool additive = false);
    void clearSelection() {
		selection.clear();
		isResizing = false;
		isDragging = false;
		isInZone = false;
	}

private:
	std::unordered_set<SceneObject *> selection;
    glm::vec2 dragOffset;
	glm::vec2 lastMouse;
	glm::vec2 zoneStart;
	glm::vec2 zoneEnd;
    bool isResizing = false;
	bool isDragging = false;
	bool isInZone = false;
    float minSize = 20.0f;
    static constexpr float handleSize = 10.0f;

	SceneObject * single() const { return selection.size() == 1 ? *selection.begin() : nullptr; }
	ofRectangle getHandleBounds(const SceneObject & object) const;
	ofRectangle getZoneBounds() const;
};
