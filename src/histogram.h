#pragma once
#include "ofMain.h"

class Histogram {
public:
	void compute(const ofImage & image);
	void draw(ofRectangle area);
	bool is_computed() const { return computed; }

private:
	std::vector<int> histoR, histoG, histoB;
	bool computed = false;

	std::vector<int> computeHistogram(const ofImage & image, int channel);
	void drawHistogram(const std::vector<int> & histogram, ofRectangle area, ofColor barColor = ofColor(255));
	void drawRgbHistogram(const std::vector<int> & histoR, const std::vector<int> & histoG, const std::vector<int> & histoB, ofRectangle area);
};
