#pragma once
#include "ofMain.h"

using namespace std;

class Histogram {
public:
	void compute(const ofImage & image);
	void draw(const ofRectangle & area) const;
	void reset();
	bool is_computed() const { return computed; }

private:
	vector<int> histoR, histoG, histoB;
	bool computed = false;

	static vector<int> computeChannel(const ofImage & image, size_t channel);
	static void drawChannel(const vector<int> & counts, const ofRectangle & area, const ofColor & barColor);
};
