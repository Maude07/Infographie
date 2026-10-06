#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "presentation/renderer.h"
#include "infrastructure/imageExporter.h"
#include "presentation/transformTool.h"
#include "presentation/ui/palettePreview.h"
#include "presentation/ui/sceneTreePanel.h"
#include "domain/histogram.h"
#include "domain/shapes/scenePrimitiveLine.h"
#include "domain/shapes/scenePrimitivePoint.h"
#include "domain/shapes/scenePrimitiveRect.h"
#include "domain/shapes/scenePrimitiveEllipse.h"

enum class VectorPrimitiveType { Select, Rect, Line, Point, Ellipse };

class Application : public ofBaseApp {

public:
	void setup() override;
	void update() override;
	void draw() override;
	void exit() override;

	void keyPressed(ofKeyEventArgs & args) override;
	void mousePressed(int x, int y, int button) override;
	void mouseDragged(int x, int y, int button) override;
	void mouseReleased(int x, int y, int button) override;
	void windowResized(int w, int h) override;

private:
	Renderer renderer;
	ImageExporter exporter;
	Histogram histogram;
	Scene scene;
	TransformTool transformTool;
	SceneTreePanel sceneTreePanel;

	vector<ofColor> palette;
	array<PalettePreview, 3> palettePreviews;
	ofColor lastActiveColor = ofColor();

	ofxPanel gui;
	ofxGuiGroup groupDraw;
	ofxGuiGroup groupExport;
	ofxGuiGroup groupTransform;

	ofxColorSlider backgroundSlider;
	ofxColorSlider strokeSlider;
	ofxColorSlider fillSlider;
	ofParameter<ofColor> backgroundColor;
	ofParameter<ofColor> strokeColor;
	ofParameter<ofColor> fillColor;
	ofParameter<float> strokeWeight;

	ofParameter<int> exportFps;
	ofParameter<float> exportDuration;

	ofParameter<string> displayText;
	ofParameter<bool> showGui;
	ofParameter<bool> showSceneTree;

	ofParameter<float> transformX;
	ofParameter<float> transformY;
	ofParameter<float> transformRotation;
	ofParameter<float> transformWidth;
	ofParameter<float> transformHeight;

	ofxButton importButton;
	ofxButton recordButton;
	ofxButton resetButton;
	ofxButton histogramButton;

	VectorPrimitiveType drawMode = VectorPrimitiveType::Select;
	glm::vec2 mousePressPos;
	glm::vec2 mouseCurrentPos;
	bool isMouseButtonPressed = false;
	bool histogramRequested = false;

	void setupTheme();
	void setupImportGui();
	void setupDrawGui();
	void setupExportGui();
	void setupMiscGui();
	void setupTransformGui();

	void syncTransformGui();
	void addColorPicker(ofxColorSlider & slider, ofParameter<ofColor> & color, PalettePreview & preview,
		const string & name, const ofColor & initialColor);

	void onColorChanged(ofColor & color);
	void onImportPressed();
	void onRecordPressed();
	void onResetPressed();
	void onHistogramPressed();
	void onTransformChanged(float & value);

	void setDrawMode(VectorPrimitiveType mode);
	void deleteSelectedObject();
	void removeSelectedPaletteColor();
	void clearPaletteSelection();

	float canvasLeft() const;
	void computeHistogram(const ofRectangle & canvas);
	void drawSidebarBackground() const;
	void drawCollapsedGuiButton() const;
	void drawRecordingIndicator() const;

	unique_ptr<ScenePrimitive> makeShape(VectorPrimitiveType type, const glm::vec2 & start, const glm::vec2 & end) const;
	void addVectorShape();
	void applyDrawStyle(ScenePrimitive & primitive) const;

	array<ofParameter<float> *, 5> transformParameters();
};
