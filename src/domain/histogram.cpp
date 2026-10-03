#include "domain/histogram.h"

using namespace std;

namespace {
	constexpr size_t levelCount = 256;
	const ofColor redBarColor(255, 0, 0, 120);
	const ofColor greenBarColor(0, 255, 0, 120);
	const ofColor blueBarColor(0, 0, 255, 120);
}

void Histogram::compute(const ofImage & image) {
	histoR = computeChannel(image, 0);
	histoG = computeChannel(image, 1);
	histoB = computeChannel(image, 2);
	computed = true;
}

void Histogram::draw(const ofRectangle & area) const {
	ofPushStyle();
	ofEnableAlphaBlending();
	drawChannel(histoR, area, redBarColor);
	drawChannel(histoG, area, greenBarColor);
	drawChannel(histoB, area, blueBarColor);
	ofPopStyle();
}

void Histogram::reset() {
	histoR.clear();
	histoG.clear();
	histoB.clear();
	computed = false;
}

vector<int> Histogram::computeChannel(const ofImage & image, size_t channel) {
	const ofPixels & pixels = image.getPixels();
	vector<int> counts(levelCount, 0);

	for (size_t y = 0; y < pixels.getHeight(); y++) {
		for (size_t x = 0; x < pixels.getWidth(); x++) {
			counts[pixels.getColor(x, y) [channel]]++;
		}
	}
	return counts;	
}

void Histogram::drawChannel(const vector<int> & counts, const ofRectangle & area, const ofColor & barColor) {
	if (counts.empty()) return;

	int maxCount = *max_element(counts.begin(), counts.end());
	if (maxCount == 0) return;

	float barWidth = area.width / counts.size();

	ofPushStyle();
	ofFill();
	ofSetColor(barColor);
	for (size_t i = 0; i < counts.size(); ++i) {
		float barHeight = static_cast<float>(counts[i]) / maxCount * area.height;
		ofDrawRectangle(area.x + i * barWidth, area.getBottom() - barHeight, barWidth, barHeight);
	}
	ofPopStyle();
}
