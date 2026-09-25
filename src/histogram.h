#pragma once
#include "ofMain.h"

using namespace std;

class Histogram {
public:
	void compute(const ofImage & image);
	void draw(ofRectangle area);
	bool is_computed() const { return computed; }

private:
	vector<int> histoR, histoG, histoB;
	bool computed = false;

	vector<int> computeHistogram(const ofImage & image, int channel);
	void drawHistogram(const vector<int> & histogram, ofRectangle area, ofColor barColor = ofColor(255));
	void drawRgbHistogram(const vector<int> & histoR, const vector<int> & histoG, const vector<int> & histoB, ofRectangle area);
};
