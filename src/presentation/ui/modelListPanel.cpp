#include "modelListPanel.h"

namespace {
const ofColor rowColor(40, 40, 46);
const ofColor rowHoverColor(60, 60, 68);
const ofColor rowSelectedColor(90, 140, 240);
const ofColor rowTextColor(230, 230, 235);
const ofColor headerTextColor(160, 160, 170);
const glm::vec2 rowTextOffset(10, 18);
}

void ModelListPanel::draw(const std::vector<std::string> & ids, const std::string & selectedId,
	const glm::vec2 & origin, float width, const glm::vec2 & mouse) const {
	ofPushStyle();

	ofSetColor(headerTextColor);
	ofDrawBitmapString("modeles 3D importes (m)", origin.x + margin, origin.y + 16);

	if (ids.empty()) {
		ofDrawBitmapString("aucun modele importe", origin.x + margin, origin.y + headerHeight + 18);
		ofPopStyle();
		return;
	}

	ofFill();
	for (size_t i = 0; i < ids.size(); ++i) {
		ofRectangle row = getRowBounds(i, origin, width);

		if (ids[i] == selectedId) {
			ofSetColor(rowSelectedColor);
		} else if (row.inside(mouse.x, mouse.y)) {
			ofSetColor(rowHoverColor);
		} else {
			ofSetColor(rowColor);
		}
		ofDrawRectangle(row);

		ofSetColor(rowTextColor);
		ofDrawBitmapString(ids[i], row.x + rowTextOffset.x, row.y + rowTextOffset.y);
	}
	ofPopStyle();
}

std::string ModelListPanel::hitTest(const std::vector<std::string> & ids,
	const glm::vec2 & origin, float width, float x, float y) const {
	for (size_t i = 0; i < ids.size(); ++i) {
		if (getRowBounds(i, origin, width).inside(x, y)) return ids[i];
	}
	return "";
}

ofRectangle ModelListPanel::getRowBounds(size_t index, const glm::vec2 & origin, float width) const {
	float y = origin.y + headerHeight + index * (rowHeight + rowGap);
	return ofRectangle(origin.x + margin, y, width - 2 * margin, rowHeight);
}
