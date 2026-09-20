// IFT3100A25_Interface/application.h
// Classe principale de l'application.
#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "renderer.h"
#include "imageExporter.h"

class Application : public ofBaseApp {
	Renderer renderer;
	ImageExporter exporter;

	ofxPanel gui;

	ofxGuiGroup group_draw;
	ofxGuiGroup group_export;

	ofParameter<ofColor> color_picker_background;
	ofParameter<ofColor> color_picker_stroke;

	ofParameter<float> slider_stroke_weight;

	ofParameter<string> textbox;

	ofParameter<bool> checkbox;

	ofParameter<int> slider_export_fps; // <-- nouveau
	ofParameter<float> slider_export_duration; // <-- nouveau
	ofxButton button_record;

	ofxButton button;

	void setup();
	void update();
	void draw();
	void exit();

	void keyReleased(int key);

	void windowResized(int w, int h);

	void button_pressed();

	void button_record_pressed();
};
