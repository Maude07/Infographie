#include "histogram.h"

void Histogram::compute(const ofImage & image) {
	histoR = computeHistogram(image, 0);
	histoG = computeHistogram(image, 1);
	histoB = computeHistogram(image, 2);
	computed = true;
}

void Histogram::draw(ofRectangle area) {
	drawRgbHistogram(histoR, histoG, histoB, area);
}

std::vector<int> Histogram::computeHistogram(const ofImage & image, int channel) {
	const ofPixels & pixels = image.getPixels();

	std::vector<int> hist(256, 0);
	int width = pixels.getWidth();
	int height = pixels.getHeight();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			ofColor c = pixels.getColor(x, y);

			unsigned char value;
			switch (channel) {
			case 0:
				value = c.r;
				break;
			case 1:
				value = c.g;
				break;
			case 2:
				value = c.b;
				break;
			case 3:
				value = c.a;
				break;
			default:
				value = c.r;
				break;
			}
			hist[value]++;
		}
	}
	return hist;
}

void Histogram::drawHistogram(const std::vector<int> & histogram, ofRectangle area, ofColor barColor) {
	if (histogram.empty()) return;

	int maxCount = *std::max_element(histogram.begin(), histogram.end());
	if (maxCount == 0) return;

	float barWidth = area.width / histogram.size();

	ofPushStyle();
	ofSetColor(barColor);
	ofFill();

	for (int i = 0; i < histogram.size(); ++i) {
		float normHeight = (float)histogram[i] / maxCount * area.height;

		float x = area.x + i * barWidth;
		float y = area.y + area.height - normHeight;

		ofDrawRectangle(x, y, barWidth, normHeight);
	}

	ofPopStyle();
}

void Histogram::drawRgbHistogram(const std::vector<int> & histoR,
	const std::vector<int> & histoG,
	const std::vector<int> & histoB, ofRectangle area) {
	ofEnableAlphaBlending();

	drawHistogram(histoR, area, ofColor(255, 0, 0, 120));
	drawHistogram(histoG, area, ofColor(0, 255, 0, 120));
	drawHistogram(histoB, area, ofColor(0, 0, 255, 120));

	ofDisableAlphaBlending();
}
