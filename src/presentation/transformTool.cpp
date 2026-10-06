#include "presentation/transformTool.h"
#include "commands/transformCommand.h"

using namespace std;

void TransformTool::mousePressed(Scene & scene, float x, float y) {
    if (selection && selection-> isResizable() && getHandleBounds().inside(x, y)) {
        isResizing = true;
		dragStartBounds = selection->getBounds();
        return;
    }

    isResizing = false;
    selection = scene.hitTest(x, y);

    if (selection) {
        dragOffset = glm::vec2(x, y) - glm::vec2(selection->getBounds().getPosition());
		dragStartBounds = selection->getBounds();
    }
}

void TransformTool::mouseDragged(float x, float y) {
    if (!selection) return;

    ofRectangle bounds = selection->getBounds();
    if (isResizing) {
        bounds.width = max(minSize, x - bounds.x);
        bounds.height = max(minSize, y - bounds.y);
    } else {
        bounds.setPosition(x - dragOffset.x, y - dragOffset.y);
    }
    selection->setBounds(bounds);
}

void TransformTool::mouseReleased() {
    isResizing = false;

	if (selection && history) {
		ofRectangle currentBounds = selection->getBounds();

		if (currentBounds.x != dragStartBounds.x || currentBounds.y != dragStartBounds.y || currentBounds.width != dragStartBounds.width || currentBounds.height != dragStartBounds.height) {

			history->push(make_unique<TransformCommand>(selection, dragStartBounds, currentBounds));
		}
	}
}

void TransformTool::drawOverlay() const {
    if (!selection) return;

    ofPushStyle();
    ofSetColor(255, 255, 0);
    ofSetLineWidth(1);

    ofNoFill();
    ofDrawRectangle(selection->getBounds());

    if (selection->isResizable()) {
        ofFill();
        ofDrawRectangle(getHandleBounds());
    }

    ofPopStyle();
}

ofRectangle TransformTool::getHandleBounds() const {
    ofRectangle bounds = selection->getBounds();
    return ofRectangle(
        bounds.getRight() - handleSize / 2.0f,
        bounds.getBottom() - handleSize / 2.0f,
        handleSize, handleSize);
}
