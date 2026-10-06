#pragma once

#include "domain/sceneObject.h"
#include "domain/scene.h"
#include "commands/historyManager.h"

class TransformTool {

public: 
    void mousePressed(Scene & scene, float x, float y);
    void mouseDragged(float x, float y);
    void mouseReleased();
    void drawOverlay() const;

    SceneObject * getSelection() const { return selection; }
    bool isOverHandle(float x, float y) const {
        return selection && selection->isResizable() && getHandleBounds().inside(x, y);
    } 
    bool isResizingSelection() const { return isResizing; }
    void select(SceneObject * object) { selection = object; isResizing = false; }
    void clearSelection() { selection = nullptr; }

	void setHistory(HistoryManager * history) { this->history = history; }

private:
    SceneObject * selection = nullptr;
    glm::vec2 dragOffset;
    bool isResizing = false;
    float minSize = 20.0f;
    static constexpr float handleSize = 10.0f;

	HistoryManager * history = nullptr;
	ofRectangle dragStartBounds;

    ofRectangle getHandleBounds() const;
};
