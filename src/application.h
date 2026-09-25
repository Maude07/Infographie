// IFT3100A25_Interface/application.h
// Classe principale de l'application.
#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "renderer.h"
#include "imageExporter.h"
#include "transformTool.h"
#include "../ofxPalettePreview.h"
#include "histogram.h"
#include "primitive/scenePrimitiveLine.h"
#include "primitive/scenePrimitivePoint.h"
#include "primitive/scenePrimitiveRect.h"

enum class VectorPrimitiveType { Select, Rect, Line, Point, Ellipse };

class Application : public ofBaseApp {

public:
	vector<ofColor> currentPalette;

	ofColor lastActiveColor = ofColor(0);

private:
	Renderer renderer;
	ImageExporter exporter;

	Histogram histogram;

	Scene scene;
	TransformTool transformTool;

	ofxPanel gui;

	ofxGuiGroup groupDraw;
	ofxGuiGroup groupExport;

	ofxColorSlider guiColorPickerBackground;
	ofParameter<ofColor> colorPickerBackground;

	ofxColorSlider guiColorPickerStroke;
	ofParameter<ofColor> colorPickerStroke;
	ofParameter<int> paletteIndex;
	ofParameter<float> sliderStrokeWeight;
	ofParameter<string> textBox;

	ofParameter<bool> checkBox;

	ofParameter<int> sliderExportFps;
	ofParameter<float> sliderExportDuration;
	ofxButton buttonRecord;

	ofxButton button;
	ofxButton buttonImport;

	vector<ofxPalettePreview> palettesPreview;

	vector<vector<ofColor>> allPalettes;

	ofxButton buttonHistogram;

	VectorPrimitiveType drawMode = VectorPrimitiveType::Select;
	glm::vec2 mousePressPos;
	glm::vec2 mouseCurrentPos;
	bool isMouseButtonPressed = false;

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
	void setupImportGui();
	void setupDrawGui();
	void setupExportGui();
	void setupMiscGui();

	//TODO: Uniformize button name
	void buttonPressed();
	void buttonRecordPressed();
	void histogramButtonPressed();
	void buttonImportPressed();

	unique_ptr<ScenePrimitive> makeShape(VectorPrimitiveType type, const glm::vec2 & start, const glm::vec2 & end) const;
	void addVectorShape();
	void applyDrawStyle(ScenePrimitive & primitive) const;
};
