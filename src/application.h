// IFT3100A25_Interface/application.h
// Classe principale de l'application.
#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "renderer.h"
#include "../ofxPalettePreview.h"

class Application : public ofBaseApp {

public:
	vector<ofColor> currentPalette;

	ofColor lastActiveColor = ofColor(0);

private:
	Renderer renderer;

	ofxPanel gui;

	ofxGuiGroup group_draw;

	ofxColorSlider gui_color_picker_background;
	ofParameter<ofColor> color_picker_background;

	ofxColorSlider gui_color_picker_stroke;
	ofParameter<ofColor> color_picker_stroke;
	ofParameter<int> palette_index;
	ofParameter<float> slider_stroke_weight;
	ofParameter<string> textbox;

	ofParameter<bool> checkbox;

	ofxButton button;
	ofxButton button_import_image;

	vector<ofxPalettePreview> palettesPreview;

	vector<vector<ofColor>> allPalettes;

	shared_ptr<SceneImage> selectedImage;
	ofVec2f dragOffset;
	bool isResizing = false;
	
	void onColorChanged(ofColor & color);

	void setup();
	void update();
	void draw();
	void exit();

	void keyReleased(int key);
	void keyPressed(int key);

	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
	void windowResized(int w, int h);

	void setupPalettes();
	void button_pressed();
	void button_import_pressed();
};
