#pragma once

#include "ofMain.h"

using namespace std;

class ModelListPanel {
public:
	void draw(const vector<string> & ids, const string & selectedId,
		const glm::vec2 & origin, float width, const glm::vec2 & mouse) const;
	string hitTest(const vector<std::string> & ids,
		const glm::vec2 & origin, float width, float x, float y) const;

private:
	static constexpr float headerHeight = 24.0f;
	static constexpr float rowHeight = 28.0f;
	static constexpr float rowGap = 2.0f;
	static constexpr float margin = 10.0f;

	ofRectangle getRowBounds(size_t index, const glm::vec2 & origin, float width) const;
};
