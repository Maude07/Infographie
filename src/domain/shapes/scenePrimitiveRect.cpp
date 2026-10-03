#include "domain/shapes/scenePrimitiveRect.h"

void ScenePrimitiveRect::draw() const {
	if (isDeleted) return;
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
