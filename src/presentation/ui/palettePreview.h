#pragma once
#include "ofMain.h"
#include "ofxGui.h"

class PalettePreview : public ofxBaseGui {
public:
	std::vector<ofColor> * activePaletteRef = nullptr;
	ofParameter<ofColor> * targetColorPickerRef = nullptr;
	int selectedIndex = -1;

	void setup(std::vector<ofColor> & paletteRef, ofParameter<ofColor>& targetPicker, float width = defaultWidth) {
		activePaletteRef = &paletteRef;
		b.width = width;
		b.height = swatchHeight;
		targetColorPickerRef = &targetPicker;
	}

	bool mousePressed(ofMouseEventArgs & args) override {
		if (!b.inside(args.x, args.y) ) {
			selectedIndex = -1;
			return false;
		}

		if (!activePaletteRef || !targetColorPickerRef || activePaletteRef->empty()) return true;


		float localX = args.x - b.x;

		float swatchWidth = b.width / activePaletteRef->size();
		int lastIndex = static_cast<int>(activePaletteRef->size()) -1;
		selectedIndex = std::clamp(static_cast<int>((args.x - b.x) / swatchWidth), 0, lastIndex);
		targetColorPickerRef->set((*activePaletteRef)[selectedIndex]);
		return true;
	}

	void render() override {
		if (!activePaletteRef || activePaletteRef->empty()) return;

		float swatchWidth = b.width / activePaletteRef->size();

		ofPushStyle();
		for (size_t i = 0; i < activePaletteRef->size(); i++) {
			ofRectangle swatch(b.x + i * swatchWidth, b.y, swatchWidth - swatchGap, b.height);

			ofFill();
			ofSetColor((*activePaletteRef)[i]);
			ofDrawRectangle(swatch);

			if (static_cast<int>(i) == selectedIndex) {
				ofNoFill();
				ofSetColor(255);
				ofSetLineWidth(selectionLineWidth);
				ofDrawRectangle(swatch);
			}
		}
		ofPopStyle();
	}

	bool setValue(float mx, float my, bool bCheck) override { return false; }
	void generateDraw() override {}

	bool mouseMoved(ofMouseEventArgs & args) override { return false; }
	bool mouseDragged(ofMouseEventArgs & args) override { return false; }
	bool mouseReleased(ofMouseEventArgs & args) override { return false; }
	bool mouseScrolled(ofMouseEventArgs & args) override { return false; }

	ofAbstractParameter & getParameter() override {
		static ofParameter<void> dummy;
		return dummy;
	}

private:
	static constexpr float defaultWidth = 200.0f;
	static constexpr float swatchHeight = 30.0f;
	static constexpr float swatchGap = 2.0f;
	static constexpr float selectionLineWidth = 2.0f;
};
