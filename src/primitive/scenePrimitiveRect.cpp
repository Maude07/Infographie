#include "scenePrimitiveRect.h"

void ScenePrimitiveRect::draw() const {
	applyStyle();
	ofDrawRectangle(position.x, position.y, size.x, size.y);
	if (filled) {
		ofNoFill();
		ofSetColor(lineColor);
		ofDrawRectangle(position.x, position.y, size.x, size.y);
	}
}
