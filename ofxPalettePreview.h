#pragma once
#include "ofMain.h"
#include "ofxGui.h"

class ofxPalettePreview : public ofxBaseGui {
public:
	std::vector<ofColor> * activePaletteRef;
	ofParameter<ofColor> * targetColorPickerRef;
	int selectedIndex = -1;

	void setup(std::vector<ofColor> & paletteRef, ofParameter<ofColor>& targetPicker, float width = 200) {
		activePaletteRef = &paletteRef;
		b.width = width;
		b.height = 30;
		targetColorPickerRef = &targetPicker;
	}

	virtual bool mousePressed(ofMouseEventArgs & args) override {
		if (b.inside(args.x, args.y) ) {

			if (!activePaletteRef || activePaletteRef->empty()) return true;


			float localX = args.x - b.x;

			float squareWidth = b.width / activePaletteRef->size();
			int clickedIndex = ofClamp(floor(localX / squareWidth), 0, activePaletteRef->size() - 1);
			selectedIndex = clickedIndex;
			targetColorPickerRef->set((*activePaletteRef)[clickedIndex]);
			return true;
		}
		selectedIndex = -1;
		return false;

	}


	virtual void render() override {
		if (!activePaletteRef || activePaletteRef->empty()) return;

		float squareWidth = b.width / activePaletteRef->size();
		ofPushStyle();

		for (size_t i = 0; i < activePaletteRef->size(); i++) {
			ofFill();
			ofSetColor((*activePaletteRef)[i]);
			ofDrawRectangle(b.x + i * squareWidth, b.y, squareWidth - 2, b.height);

			//Ligne de contour pour carré sélectionné
			if (i == selectedIndex) {
				ofNoFill();
				ofSetColor(255); //Border blanche
				ofSetLineWidth(2);
				ofDrawRectangle(b.x + i * squareWidth, b.y, squareWidth - 2, b.height);
			}
		}

		ofPopStyle();
	}

	virtual bool setValue(float mx, float my, bool bCheck) override { return false; }
	virtual void generateDraw() override { }

	//Obligatoire pour que le code compile, mais pas utilisé dans ce widget
	virtual bool mouseMoved(ofMouseEventArgs & args) override { return false; }
	virtual bool mouseDragged(ofMouseEventArgs & args) override { return false; }
	virtual bool mouseReleased(ofMouseEventArgs & args) override { return false; }
	virtual bool mouseScrolled(ofMouseEventArgs & args) override { return false; }

	virtual ofAbstractParameter & getParameter() override {
		static ofParameter<void> dummy;
		return dummy;
	}
};
