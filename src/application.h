// IFT3100A25_Interface/application.h
// Classe principale de l'application.
#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "renderer.h"
#include "../ofxPalettePreview.h"
#include "histogram.h"

class Application : public ofBaseApp {

public:
	vector<ofColor> currentPalette;

	ofColor lastActiveColor = ofColor(0);

private:
	Renderer renderer;

	Histogram histogram;

	ofxPanel gui;

	ofxGuiGroup group_draw;

	ofxColorSlider gui_color_picker_background;
	//ofxGuiGroup background_color_group;
	ofParameter<ofColor> color_picker_background;

	ofxColorSlider gui_color_picker_stroke;
	ofParameter<ofColor> color_picker_stroke;
	ofParameter<int> palette_index;
	ofParameter<float> slider_stroke_weight;
	ofParameter<string> textbox;

	ofParameter<bool> checkbox;

	ofxButton button;

	vector<ofxPalettePreview> palettesPreview;

	vector<vector<ofColor>> allPalettes;

	ofxButton button_histogram;

	void onColorChanged(ofColor & color);


	void setup();
	void update();
	void draw();
	void exit();

	void keyReleased(int key);
	void keyPressed(int key);

	void windowResized(int w, int h);

	void setupPalettes();
	void button_pressed();

	void histogram_button_pressed();
};
