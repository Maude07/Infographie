#include "scenePrimitiveRect.h"

void ScenePrimitiveRect::draw() const {
	ofPushStyle();
	applyStyle();
	ofDrawRectangle(position.x, position.y, size.x, size.y);
	if (filled) {
		ofNoFill();
		ofSetColor(lineColor);
		ofDrawRectangle(position.x, position.y, size.x, size.y);
	}
	ofPopStyle();
}
