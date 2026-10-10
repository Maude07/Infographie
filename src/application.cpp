#include "application.h"
#include "domain/sceneImage.h"
#include "domain/shapes/scenePrimitiveEllipse.h"
#include "domain/shapes/scenePrimitiveLine.h"
#include "domain/shapes/scenePrimitivePoint.h"
#include "domain/shapes/scenePrimitiveRect.h"

using namespace std;

namespace {

	//TODO : Default UI should be in another file?
// Theme
const ofColor sidebarColor(18, 18, 22);
const ofColor sidebarBorderColor(60, 60, 68);
const ofColor panelColor(24, 24, 28);
const ofColor panelBorderColor(45, 45, 50);
const ofColor accentColor(90, 140, 240);
const ofColor textColor(230, 230, 235);
constexpr int guiWidth = 280;
constexpr int guiRowHeight = 36;
constexpr int guiFontSize = 13;
const string guiFontPath = "fonts/static/OpenSans-Regular.ttf";

// Default values, also restored by the reset button
const ofColor defaultBackgroundColor(31);
const ofColor defaultStrokeColor(255);
const ofColor defaultFillColor(31);
constexpr float defaultStrokeWeight = 4.0f;
const string defaultText = "ift3100";

vector<ofColor> defaultPalette() {
	return {
		ofColor(15, 15, 15),
		ofColor(255, 182, 193),
		ofColor(143, 131, 216),
		ofColor(155, 184, 237)
	};
}

// Button shown in the top-left corner while the panel is hidden
const ofRectangle collapsedGuiButton(10, 10, 30, 30);
const glm::vec2 collapsedGuiIconOffset(10, 18);

// Imported images are placed in a cascade so they don't land exactly on top of each other
constexpr float importMarginLeft = 50.0f;
constexpr float importTop = 100.0f;
constexpr float importCascadeOffset = 30.0f;
constexpr size_t importCascadeSteps = 10;

// Histogram overlay, anchored to the bottom-left of the canvas
constexpr float histogramMargin = 20.0f;
constexpr float histogramWidth = 300.0f;
constexpr float histogramHeight = 120.0f;
constexpr float histogramMarginBottom = 30.0f;

// Recording indicator, anchored to the top-right of the window
constexpr float recordingIndicatorRight = 120.0f;
constexpr float recordingIndicatorTop = 30.0f;
constexpr float recordingIndicatorRadius = 8.0f;
const glm::vec2 recordingLabelOffset(15, 5);

constexpr float minShapeSize = 3.0f;

bool isLargeEnough(VectorPrimitiveType type, const glm::vec2 & start, const glm::vec2 & end) {
	glm::vec2 extent = glm::abs(end - start);
	switch (type) {
		case VectorPrimitiveType::Rect:
		case VectorPrimitiveType::Ellipse:
			return extent.x >= minShapeSize && extent.y >= minShapeSize;
		case VectorPrimitiveType::Line:
			return glm::length(extent) >= minShapeSize;
		case VectorPrimitiveType::Point:
		case VectorPrimitiveType::Select:
			return true;
	}
		return true;
}

string shapeLabel(VectorPrimitiveType type) {
	switch (type) {
		case VectorPrimitiveType::Rect: return "rectangle";
		case VectorPrimitiveType::Line: return "ligne";
		case VectorPrimitiveType::Point: return "point";
		case VectorPrimitiveType::Ellipse: return "ellipse";
		case VectorPrimitiveType::Select: break;;
	}
	return "objet";
}

}

void Application::setup() {

#ifdef _WIN32
	FILE * console;
	freopen_s(&console, "CONOUT$", "w", stdout);
	freopen_s(&console, "CONOUT$", "w", stderr);
#endif

	ofSetWindowTitle("interface (u) graphe (i) ");
	ofLog() << "<app::setup>";

	setupTheme();
	renderer.setup();
	cursor.setup();

	gui.setup("interface", "setting.json", 0, 0);
	palette = defaultPalette();

	setupImportGui();
	setupDrawGui();
	setupExportGui();
	setupMiscGui();
}

void Application::setupTheme() {
	ofxGuiSetDefaultWidth(guiWidth);
	ofxGuiSetDefaultHeight(guiRowHeight);
	ofxGuiSetFillColor(accentColor);
	ofxGuiSetBackgroundColor(panelColor);
	ofxGuiSetBorderColor(panelBorderColor);
	ofxGuiSetHeaderColor(sidebarColor);
	ofxGuiSetTextColor(textColor);
	ofxGuiSetFont(guiFontPath, guiFontSize);
}

void Application::setupImportGui() {
	importButton.setup("Importer une image");
	importButton.addListener(this, &Application::onImportPressed);
	gui.add(&importButton);
}

void Application::setupDrawGui() {
	groupDraw.setup("outils de dessin");

	addColorPicker(backgroundSlider, backgroundColor, palettePreviews[0], "couleur du canevas", defaultBackgroundColor);
	addColorPicker(strokeSlider, strokeColor, palettePreviews[1], "couleur du trait", defaultStrokeColor);
	addColorPicker(fillSlider, fillColor, palettePreviews[2], "couleur de remplissage", defaultFillColor);
	groupDraw.add(strokeWeight.set("largeur de la ligne", defaultStrokeWeight, 0.0f, 10.0f));

	gui.add(&groupDraw);
}

void Application::addColorPicker(ofxColorSlider & slider, ofParameter<ofColor> & color, PalettePreview & preview,
	const string & name, const ofColor & initialColor) {
		color.set(name, initialColor, ofColor(0, 0), ofColor(255, 255));
		slider.setup(color);
		preview.setup(palette, color, groupDraw.getWidth());
		slider.add(&preview);
		groupDraw.add(&slider);
		color.addListener(this, &Application::onColorChanged);
	}

void Application::setupExportGui() {
	groupExport.setup("exportation d'images");
	groupExport.add(exportFps.set("images/sec", 24, 1, 60));
	groupExport.add(exportDuration.set("duree (s, 0=illimite)", 5.0f, 0.0f, 60.0f));
	
	recordButton.setup("enregistrer / arreter (r)");
	recordButton.addListener(this, &Application::onRecordPressed);
	groupExport.add(&recordButton);

	gui.add(&groupExport);
}

void Application::setupMiscGui() {
	gui.add(displayText.set("text", defaultText));

	resetButton.setup("Reinitialiser");
	resetButton.addListener(this, &Application::onResetPressed);
	gui.add(&resetButton);

	gui.add(showGui.set("visible", true));
	gui.add(showSceneTree.set("graphe de scene (i)", false));

	histogramButton.setup("Calculer l'histogramme");
	histogramButton.addListener(this, &Application::onHistogramPressed);
	gui.add(&histogramButton);
}

void Application::update() {
	renderer.backgroundColor = backgroundColor;
	renderer.strokeColor = strokeColor;
	renderer.strokeWeight = strokeWeight;
	renderer.text = displayText;
	renderer.update();
}

void Application::draw() {
	float left = canvasLeft();
	ofRectangle canvas(left, 0, ofGetWidth() - left, ofGetHeight());

	renderer.visibleOffsetX = left;
	renderer.draw(scene);

	if (histogramRequested) {
		computeHistogram(canvas);
	}

	exporter.captureX = static_cast<int>(canvas.x);
	exporter.captureY = static_cast<int>(canvas.y);
	exporter.captureWidth = static_cast<int>(canvas.width);
	exporter.captureHeight = static_cast<int>(canvas.height);
	exporter.capture();

	if (showGui) {
		drawSidebarBackground();
	}

	transformTool.drawOverlay();

	if (isMouseButtonPressed) {
		if (auto preview = makeShape(drawMode, mousePressPos, mouseCurrentPos)) {
			preview->draw();
		}
	}

	if (exporter.isRecording) {
		drawRecordingIndicator();
	}

	if (histogram.is_computed()) {
		histogram.draw(ofRectangle(left + histogramMargin, ofGetHeight() - histogramMarginBottom - histogramHeight,
			histogramWidth, histogramHeight));
	}

	if (showSceneTree) {
		sceneTreePanel.draw(scene, transformTool.getSelection(), glm::vec2(ofGetMouseX(), ofGetMouseY()));
	}

	if (showGui) {
		gui.draw();
	} else {
		drawCollapsedGuiButton();
	}

	glm::vec2 mouse(ofGetMouseX(), ofGetMouseY());
	cursor.draw(currentCursorState(mouse), mouse);
}

void Application::computeHistogram(const ofRectangle & canvas) {
	histogramRequested = false;

	ofImage capture;
	capture.grabScreen(static_cast<int>(canvas.x), static_cast<int>(canvas.y),
		static_cast<int>(canvas.width), static_cast<int>(canvas.height));

	if (!capture.isAllocated()) {
		ofLogWarning() << "histogramme: capture échouée";
		return;
	}
	histogram.compute(capture);
	ofLog() << "histogramme calculé";
}

void Application::drawSidebarBackground() const {
	ofPushStyle();
	ofFill();
	ofSetColor(sidebarColor);
	ofDrawRectangle(0, 0, gui.getWidth(), ofGetHeight());
	ofSetColor(sidebarBorderColor);
	ofDrawLine(gui.getWidth(), 0, gui.getWidth(), ofGetHeight());
	ofPopStyle();
}

void Application::drawCollapsedGuiButton() const {
	ofPushStyle();
	ofFill();
	ofSetColor(panelColor);
	ofDrawRectangle(collapsedGuiButton);

	ofNoFill();
	ofSetColor(accentColor);
	ofSetLineWidth(2);
	ofDrawRectangle(collapsedGuiButton);

	ofFill();
	ofSetColor(textColor);
	ofDrawBitmapString(">", collapsedGuiButton.getPosition() + glm::vec3(collapsedGuiIconOffset, 0));
	ofPopStyle();
}

void Application::drawRecordingIndicator() const {
	glm::vec2 center(ofGetWidth() - recordingIndicatorRight, recordingIndicatorTop);

	ofPushStyle();
	ofFill();
	ofSetColor(255, 0, 0);
	ofDrawCircle(center, recordingIndicatorRadius);
	ofSetColor(255);
	ofDrawBitmapString("REC " + ofToString(exporter.frameCount), center + recordingLabelOffset);
	ofPopStyle();
}

CursorState Application::currentCursorState(const glm::vec2 & mouse) const {
	if (showGui && gui.getShape().inside(mouse)) return CursorState::Default;
	if (!showGui && collapsedGuiButton.inside(mouse)) return CursorState::Default;
	if (showSceneTree && sceneTreePanel.hitTest(scene, mouse.x, mouse.y)) return CursorState::Hover;

	switch (drawMode) {
		case VectorPrimitiveType::Rect:
		case VectorPrimitiveType::Line:
		case VectorPrimitiveType::Ellipse:
			return CursorState::Crosshair;
		case VectorPrimitiveType::Point:
			return CursorState::Point;
		case VectorPrimitiveType::Select:
			break;
	}

	if (transformTool.isResizingSelection() || transformTool.isOverHandle(mouse.x, mouse.y)) {
		return CursorState::Resize;
	}
	if (isMouseButtonPressed && transformTool.hasSelection()) return CursorState::Move;
	if (scene.hitTest(mouse.x , mouse.y)) return CursorState::Hover;

	return CursorState::Default;
}

void Application::onColorChanged(ofColor & color) {
	lastActiveColor = color;
}

void Application::keyPressed(ofKeyEventArgs & args) {
	if (args.isRepeat) return;

	switch (args.key) {
	case 'u':
		showGui = !showGui;
		ofLog() << "<toggle ui: " << showGui << ">";
		break;
	
	case 'i':
		showSceneTree = !showSceneTree;
		ofLog() << "<toggle scene tree: " << showSceneTree << ">";
		break;

	case 'r':
		onRecordPressed();
		break;

	case '1':
		setDrawMode(VectorPrimitiveType::Select);
		break;
	case '2':
		setDrawMode(VectorPrimitiveType::Rect);
		break;
	case '3':
		setDrawMode(VectorPrimitiveType::Line);
		break;
	case '4':
		setDrawMode(VectorPrimitiveType::Point);
		break;
	case '5':
		setDrawMode(VectorPrimitiveType::Ellipse);
		break;

	case OF_KEY_DEL :
	case OF_KEY_BACKSPACE :
		if (transformTool.hasSelection()) {
			deleteSelectedObject();
		} else {
			removeSelectedPaletteColor();
		}
		break;

	case OF_KEY_RETURN:
		palette.push_back(lastActiveColor);
		ofLog() << "<added color: " << lastActiveColor << ">";
		break;
	}
}

void Application::deleteSelectedObject() {
	auto toRemove = transformTool.getSelection();
	ofLog() << "<deleted objects: " << toRemove.size() << ">";
	transformTool.clearSelection();
	scene.remove(toRemove);
}

void Application::setDrawMode(VectorPrimitiveType mode) {
	drawMode = mode;
	transformTool.clearSelection();
	isMouseButtonPressed = false;
}

void Application::removeSelectedPaletteColor() {

	for (const auto & preview : palettePreviews) {
		int index = preview.selectedIndex;
		if (index < 0 || index >= static_cast<int>(palette.size())) continue;

		palette.erase(palette.begin() + index);
		ofLog() << "<deleted color at index: " << index << ">";
		break;
	}

	clearPaletteSelection();
}

void Application::clearPaletteSelection() {
	for (auto & preview : palettePreviews) {
		preview.selectedIndex = -1;
	}
}

void Application::mousePressed(int x, int y, int button) {
	if (!showGui && collapsedGuiButton.inside(x, y)) {
		showGui = true;
		return;
	}

	if (showGui && gui.getShape().inside(x, y)) return;

	bool additive = ofGetKeyPressed(OF_KEY_SHIFT) || ofGetKeyPressed(OF_KEY_CONTROL);

	if (showSceneTree) {
		if (SceneObject * clicked = sceneTreePanel.hitTest(scene, x, y)) {
			if (drawMode != VectorPrimitiveType::Select) setDrawMode(VectorPrimitiveType::Select);
			transformTool.select(clicked, additive);
			return;
		}
	}

	isMouseButtonPressed = true;

	if (drawMode == VectorPrimitiveType::Select) {
		transformTool.mousePressed(scene, x, y, additive);
		return;
	}

	mousePressPos = mouseCurrentPos = glm::vec2(x, y);
}
	
void Application::mouseDragged(int x, int y, int button) {
	if (!isMouseButtonPressed) return;

	if (drawMode == VectorPrimitiveType::Select) {
		transformTool.mouseDragged(x, y);
		return;
	}
	mouseCurrentPos = glm::vec2(x, y);
}

void Application::mouseReleased(int x, int y, int button) {
	if(!isMouseButtonPressed) return;
	isMouseButtonPressed = false;

	if (drawMode == VectorPrimitiveType::Select) {
		transformTool.mouseReleased(scene);
		return;
	}
	mouseCurrentPos = glm::vec2(x, y);
	addVectorShape();

}

void Application::onResetPressed() {
	if (exporter.isRecording) exporter.stop();
	setDrawMode(VectorPrimitiveType::Select);
	scene.clear();
	histogram.reset();
	histogramRequested = false;

	displayText.set(defaultText);
	backgroundColor = defaultBackgroundColor;
	strokeColor = defaultStrokeColor;
	fillColor = defaultFillColor;
	strokeWeight = defaultStrokeWeight;

	palette = defaultPalette();
	clearPaletteSelection();

	ofLog() << "button pressed>";
}

void Application::onImportPressed() {
	ofFileDialogResult result = ofSystemLoadDialog("Choisir une image");
	if (!result.bSuccess) return;
	
	auto image = SceneImage::load(result.getPath());
	if (!image) return;

	float offset = importCascadeOffset * (scene.size() % importCascadeSteps);
	image->position = { canvasLeft() + importMarginLeft + offset, importTop + offset };
	scene.add(move(image));
}

void Application::onRecordPressed() {
	exporter.fps = exportFps;
	exporter.duration = exportDuration;
	exporter.toggle();
}

void Application::onHistogramPressed() {
	histogramRequested = true;
}

void Application::windowResized(int w, int h) {
	ofLog() << "<app::windowResized: (" << w << ", " << h << ")>";
}

unique_ptr<ScenePrimitive> Application::makeShape(VectorPrimitiveType type, const glm::vec2& start, const glm::vec2& end) const {
	unique_ptr<ScenePrimitive> shape;

	switch (type) {
	case VectorPrimitiveType::Line:
		shape = make_unique<ScenePrimitiveLine>();
		shape->SceneObject::position = start;
		shape->size = end;
		shape->filled = false;
		break;
	case VectorPrimitiveType::Point:
		shape = make_unique<ScenePrimitivePoint>();
		shape->SceneObject::position = start;
		shape->size = end;
		shape->filled = false;
		break;
	case VectorPrimitiveType::Rect:
		shape = make_unique<ScenePrimitiveRect>();
		shape->SceneObject::position = glm::min(start, end);
		shape->size = glm::abs(end - start);
		break;
	case VectorPrimitiveType::Ellipse:
		shape = make_unique<ScenePrimitiveEllipse>();
		shape->SceneObject::position = (start + end) * 0.5f;
		shape->size = glm::abs(end - start);
		break;
	case VectorPrimitiveType::Select:
		return nullptr;
	}

	shape->name = shapeLabel(type);
	applyDrawStyle(*shape);
	return shape;
}

void Application::addVectorShape() {
	if (!isLargeEnough(drawMode, mousePressPos, mouseCurrentPos)) return;

	if (auto shape = makeShape(drawMode, mousePressPos, mouseCurrentPos)) {
		scene.add(move(shape));
	}
}

void Application::applyDrawStyle(ScenePrimitive& primitive) const {
	primitive.fillColor = fillColor;
	primitive.lineColor = strokeColor;
	primitive.lineWidth = strokeWeight;
}

float Application::canvasLeft() const {
	return showGui ? gui.getWidth() : 0.0f;
}


void Application::exit() {
	importButton.removeListener(this, &Application::onImportPressed);
	recordButton.removeListener(this, &Application::onRecordPressed);
	resetButton.removeListener(this, &Application::onResetPressed);
	histogramButton.removeListener(this, &Application::onHistogramPressed);

	backgroundColor.removeListener(this, &Application::onColorChanged);
	strokeColor.removeListener(this, &Application::onColorChanged);
	fillColor.removeListener(this, &Application::onColorChanged);

	cursor.exit();

	ofLog() << "<app::exit>";
}
