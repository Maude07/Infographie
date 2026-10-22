#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "presentation/cursorRenderer.h"
#include "presentation/renderer.h"
#include "infrastructure/imageExporter.h"
#include "presentation/bezierStrokeTool.h"
#include "presentation/transformTool.h"
#include "presentation/ui/palettePreview.h"
#include "presentation/ui/sceneTreePanel.h"
#include "presentation/ui/modelListPanel.h"
#include "domain/histogram.h"
#include "domain/camera/sceneCamera.h"
#include "domain/camera/cameraController.h"
#include "domain/shapes/2d/scenePrimitiveLine.h"
#include "domain/shapes/2d/scenePrimitivePoint.h"
#include "domain/shapes/2d/scenePrimitiveRect.h"
#include "domain/shapes/2d/scenePrimitiveEllipse.h"
#include "vector_shapes/sceneShapeFunkyCircles.h"
#include "vector_shapes/sceneShapeStars.h"
#include "vector_shapes/sceneShapeWave.h"
#include "infrastructure/modelLibrary.h"
#include "domain/shapes/3d/sceneModel3d.h"
#include "domain/shapes/3d/scenePrimitiveCube.h"
#include "domain/shapes/3d/scenePrimitiveSphere.h"
#include "domain/shapes/3d/scenePrimitiveCylinder.h"
#include "domain/shapes/3d/scenePrimitiveCone.h"
#include "domain/shapes/3d/scenePrimitive3d.h"
#include "domain/commands/historyManager.h"

enum class Primitive3dType {
	Cube,
	Sphere,
	Cylinder,
	Cone
};

enum class ToolType {
	Select,
	Rect,
	Line,
	Point,
	Ellipse,
	Stars,
	Wave,
	FunkyCircles,
	Bezier,
	Model3d,
	Primitive3d
};

class Application : public ofBaseApp {

public:
	void setup() override;
	void update() override;
	void draw() override;
	void exit() override;

	void keyPressed(ofKeyEventArgs & args) override;
	void keyReleased(ofKeyEventArgs & args) override;
	void mousePressed(int x, int y, int button) override;
	void mouseDragged(int x, int y, int button) override;
	void mouseReleased(int x, int y, int button) override;
	void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
	void windowResized(int w, int h) override;

private:
	Renderer renderer;
	ImageExporter exporter;
	Histogram histogram;
	Scene scene;
	TransformTool transformTool;
	BezierStrokeTool bezierStrokeTool;
	SceneTreePanel sceneTreePanel;
	CursorRenderer cursor;
	ModelLibrary modelLibrary;
	string currentModelId;
	Primitive3dType currentPrimitive3d = Primitive3dType::Cube;
	ModelListPanel modelListPanel;
	HistoryManager history;
	ModelVariant currentVariant;

	ofParameter<int> variantIndex;
	float scrollOffset = 0.0f;
	float maxScroll = 0.0f;
	static constexpr float scrollSpeed = 40.0f;
	static constexpr float scrollbarTrackWidth = 24.0f;
	static constexpr float scrollbarThumbWidth = 10.0f;
	bool isDraggingScrollbar = false;
	float scrollbarDragOffsetY = 0.0f;

	vector<ofColor> palette;
	array<PalettePreview, 4> palettePreviews;
	ofColor lastActiveColor = ofColor();

	ofColor colorRefBackground;
	ofColor colorRefStroke;
	ofColor colorRefFill;
	bool wasMousePressedLastFrame = false;

	ofxPanel gui;
	ofxPanel inspector;
	ofxGuiGroup groupDraw;
	ofxGuiGroup groupExport;
	ofxGuiGroup groupTransform;

	ofxGuiGroup groupHistory;
	ofxGuiGroup groupVariants;
	ofParameter<bool> variantStretchVertical;
	ofParameter<float> variantVerticalFactor;
	ofParameter<bool> variantStretchHorizontal;
	ofParameter<float> variantHorizontalFactor;
	ofParameter<bool> variantTinted;
	ofParameter<ofColor> variantTintColor;
	ofxColorSlider variantTintSlider;
	ofParameter<bool> variantMetallic;
	ofParameter<float> variantMetalness;

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

	// 6 cameras distinctes autour de l'objet, chacune avec ses propres attributs
	static constexpr size_t cameraCount = 6;
	array<SceneCamera, cameraCount> cameras { {
		SceneCamera("face", 0.f, 0.f),
		SceneCamera("arriere", 180.f, 0.f),
		SceneCamera("cote gauche", -90.f, 0.f),
		SceneCamera("cote droit", 90.f, 0.f),
		SceneCamera("dessus", 0.f, 89.f),
		SceneCamera("dessous", 0.f, -89.f),
	} };
	size_t activeCameraIndex = 0;
	SceneCamera & activeCamera() { return cameras[activeCameraIndex]; }
	const SceneCamera & activeCamera() const { return cameras[activeCameraIndex]; }

	CameraController cameraController { cameras[0] };
	ofxGuiGroup groupCamera;
	ofParameter<int> cameraSelector;
	ofParameter<string> cameraNameLabel;
	ofxLabel cameraNameLabelGui;
	ofxButton frameCameraButton;
	ofxButton showAllButton;
	ofxButton resetCameraButton;
	ofParameter<float> cameraFov;
	ofParameter<float> cameraNear;
	ofParameter<float> cameraFar;
	ofParameter<bool> cameraOrtho;
	ofParameter<float> transformX;
	ofParameter<float> transformY;
	ofParameter<float> transformRotation;
	ofParameter<float> transformWidth;
	ofParameter<float> transformHeight;

	ofxButton importButton;
	ofxButton recordButton;
	ofxButton resetButton;
	ofxButton histogramButton;
	ofxButton importModelButton;
	ofxButton buttonUndo;
	ofxButton buttonRedo;

	ToolType toolType = ToolType::Select;
	glm::vec2 mousePressPos;
	glm::vec2 mouseCurrentPos;
	bool isMouseButtonPressed = false;
	bool histogramRequested = false;
	bool isSyncingTransformGui = false;
	SceneObject * transformEditTarget = nullptr;
	TransformState transformEditStart;

	void setupTheme();
	void setupImportGui();
	void setupDrawGui();
	void setupExportGui();
	void setupMiscGui();
	void setupTransformGui();
	void placeInspector();

	void syncTransformGui();
	void addColorPicker(ofxColorSlider & slider, ofParameter<ofColor> & color, PalettePreview & preview,
		const string & name, const ofColor & initialColor);
	glm::vec2 modelListOrigin() const;

	void onColorChanged(ofColor & color);
	void onImportPressed();
	void onRecordPressed();
	void onResetPressed();
	void onHistogramPressed();
	void onImportModelPressed();
	void onTransformChanged(float & value);

	void setupHistoryGui();
	void buttonUndoPressed();
	void buttonRedoPressed();
	void commitColorChanges();
	void commitTransformChanges();
	void syncColorReferences();

	void setupVariantGui();
	void resetVariantGui();
	ModelVariant variantFromGui() const;

	void updateSidebarScroll();
	void drawSidebarScrollbar() const;
	bool isOverScrollbarThumb(int x, int y) const;
	void setScrollFromThumbY(float thumbScreenY);

	void setToolType(ToolType mode);
	void deleteSelectedObject();
	void removeSelectedPaletteColor();
	void clearPaletteSelection();

	float canvasLeft() const;
	float canvasRight() const;
	void computeHistogram(const ofRectangle & canvas);
	void drawSidebarBackground() const;
	void drawCollapsedGuiButton() const;
	void drawRecordingIndicator() const;
	CursorState currentCursorState(const glm::vec2 & mouse) const;

	unique_ptr<ScenePrimitive> makeShape(ToolType type, const glm::vec2 & start, const glm::vec2 & end) const;
	void addVectorShape();
	void applyDrawStyle(ScenePrimitive & primitive) const;

	unique_ptr<SceneModel3d> makeModel(const glm::vec2 & start, const glm::vec2 & end) const;
	void addModel();
	void selectNextModel();
	void rotateSelectedModels(float degrees);
	bool moveSelectedModelsDepth(float delta);

	void setupCameraGui();
	void resetCamera();
	void selectCamera(size_t index);
	void onCameraSelectorChanged(int & value);
	void loadCameraAttributesToGui();
	void frameCameraOnObject();
	bool isolateSelection();
	void showWholeScene();
	void syncModelCameras();
	void drawCameraLabel() const;
	bool isOverUi(int x, int y) const;

	unique_ptr<ScenePrimitive3d> makePrimitive3d(const glm::vec2 & start, const glm::vec2 & end) const;
	void addPrimitive3d();
	void selectPrimitive3d(Primitive3dType type);

	array<ofParameter<float> *, 5> transformParameters();
};
