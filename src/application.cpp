// IFT3100A25_Interface/application.cpp
// Classe principale de l'application.

#include "application.h"
#include "sceneImage.h"

using namespace std;

void Application::setup() {
	ofSetWindowTitle("interface (u)");
	ofFill();

	ofLog() << "<app::setup>";

	renderer.setup();

	gui.setup("interface");

	setupPalettes();
	paletteIndex.set("Palette", 0, 0, allPalettes.size() -1);
	currentPalette = allPalettes[paletteIndex];

	setupImportGui();
	setupDrawGui();
	setupExportGui();
	setupMiscGui();
}

void Application::setupImportGui() {
	buttonImport.setup("Importer une image");
	buttonImport.addListener(this, &Application::buttonImportPressed);
	gui.add(&buttonImport);
}

void Application::setupDrawGui() {
	groupDraw.setup("outils de dessin");

	//Palette 1
	colorPickerBackground.set("couleur du canevas", ofColor(31), ofColor(0, 0), ofColor(255, 255));	
	guiColorPickerBackground.setup(colorPickerBackground);
	palettesPreview[0].setup(currentPalette, colorPickerBackground, groupDraw.getWidth());
	guiColorPickerBackground.add(&palettesPreview[0]);
	groupDraw.add(&guiColorPickerBackground);

	//Palette 2
	colorPickerStroke.set("couleur du trait", ofColor(255), ofColor(0, 0), ofColor(255, 255));
	guiColorPickerStroke.setup(colorPickerStroke);
	palettesPreview[1].setup(currentPalette, colorPickerStroke, groupDraw.getWidth());
	guiColorPickerStroke.add(&palettesPreview[1]);
	groupDraw.add(&guiColorPickerStroke);

	sliderStrokeWeight.set("largeur de la ligne", 4.0f, 0.0f, 10.0f);

	gui.add(&groupDraw);

	colorPickerBackground.addListener(this, &Application::onColorChanged);
	colorPickerStroke.addListener(this, &Application::onColorChanged);
}

void Application::setupExportGui() {
	groupExport.setup("exportation d'images");
	groupExport.add(sliderExportFps.set("images/sec", 24, 1, 60));
	groupExport.add(sliderExportDuration.set("duree (s, 0=illimite)", 5.0f, 0.0f, 60.0f));
	buttonRecord.setup("enregistrer / arreter (r)");
	buttonRecord.addListener(this, &Application::buttonRecordPressed);
	groupExport.add(&buttonRecord);
	gui.add(&groupExport);
}

void Application::setupMiscGui() {
	textBox.set("text", "ift3100");
	gui.add(textBox);

	button.setup("Reinitialiser");
	button.addListener(this, &Application::buttonPressed);
	gui.add(&button);

	checkBox.setName("visible");
	gui.add(checkBox);
	checkBox = true;

	currentPalette = allPalettes[paletteIndex];

	buttonHistogram.setup("Calculer l'histogramme");
	buttonHistogram.addListener(this, &Application::histogramButtonPressed);
	gui.add(&buttonHistogram);
}

void Application::update() {
	renderer.backgroundColor = colorPickerBackground;
	renderer.strokeColor = colorPickerStroke;
	renderer.strokeWeight = sliderStrokeWeight;
	renderer.text = textBox;

	currentPalette = allPalettes[0];

	renderer.update();
}

void Application::draw() {
	renderer.draw(scene);

	exporter.capture();

	transformTool.drawOverlay();

	if (checkBox)
		gui.draw();

	if (exporter.isRecording) {
		ofSetColor(255, 0, 0);
		ofFill();
		ofDrawCircle(ofGetWidth() - 120, 30, 8);
		ofSetColor(255);
		ofDrawBitmapString("REC " + ofToString(exporter.frameCount), ofGetWidth() - 105, 35);
		ofNoFill();
	}

	if (histogram.is_computed()) {
		histogram.draw(ofRectangle(gui.getPosition().x, gui.getHeight() + 10, gui.getWidth(), 100));
	}

}

void Application::onColorChanged(ofColor & color) {
	lastActiveColor = color;
}

void Application::setupPalettes() {
	allPalettes.resize(2);
	palettesPreview.resize(2);

	allPalettes[0] = {
		ofColor(15, 15, 15),
		ofColor(255, 182, 193),
		ofColor(143, 131, 216),
		ofColor(155, 184, 237)
	};
}


void Application::keyReleased(int key) {
	if (key == 117) //touche u
	{
		checkBox = !checkBox;
		ofLog() << "<toggle ui: " << checkBox << ">";
	}

	if (key == 114) {
		buttonRecordPressed(); // touche r
	}
}

void Application::keyPressed(int key) {

	if (key == OF_KEY_DEL || key == OF_KEY_BACKSPACE) {
		int activePreviewIdx = -1;
		int colorIdx = -1;

		for (size_t i = 0; i < palettesPreview.size(); i++) {
			if (palettesPreview[i].selectedIndex >= 0) {
				activePreviewIdx = i;
				colorIdx = palettesPreview[i].selectedIndex;
				break;
			}
		}

		if (colorIdx >= 0 && colorIdx < currentPalette.size()) {
			allPalettes[0].erase(allPalettes[0].begin() + colorIdx);

			palettesPreview[paletteIndex].selectedIndex = -1;

			ofLog() << "<deleted color at index: " << colorIdx << ">";
		}
	}

	if (key == OF_KEY_RETURN) {
		allPalettes[paletteIndex].push_back(lastActiveColor);

		ofLog() << "<added color: " << lastActiveColor << ">";
	}
}

void Application::mousePressed(int x, int y, int button) {
	if (checkBox && gui.getShape().inside(x, y)) return;

	transformTool.mousePressed(scene, x, y);
}
	
void Application::mouseDragged(int x, int y, int button) {
	transformTool.mouseDragged(x, y);
}

void Application::mouseReleased(int x, int y, int button) {
	transformTool.mouseReleased();
}

void Application::buttonPressed() {
	textBox.set("text", "ift3100");

	ofLog() << "<button pressed>";
}

void Application::buttonImportPressed() {
	ofFileDialogResult result = ofSystemLoadDialog("Choisir une image");
	if (!result.bSuccess) return;
	
	if (auto image = SceneImage::load(result.getPath())) {
		float offset = 30.0f * scene.size();
		image->position = { 100 + offset, 100 + offset };
		scene.add(move(image));
	}
}

void Application::buttonRecordPressed() {
	exporter.fps = sliderExportFps;
	exporter.duration = sliderExportDuration;
	exporter.toggle();
}

void Application::windowResized(int w, int h) {
	ofLog() << "<app::windowResized: (" << w << ", " << h << ")>";
}

void Application::exit() {
	button.removeListener(this, &Application::buttonPressed);
	buttonRecord.removeListener(this, &Application::buttonRecordPressed);
	buttonHistogram.removeListener(this, &Application::histogramButtonPressed);
	buttonImport.removeListener(this, &Application::buttonImportPressed);

	ofLog() << "<app::exit>";
}

void Application::histogramButtonPressed() {
	ofImage capture;
	capture.grabScreen(gui.getWidth() + 10, 0, ofGetWidth() - gui.getWidth() - 10, ofGetHeight());

	if (!capture.isAllocated()) {
		ofLogWarning() << "histograme: capture échouée";
		return;
	}

	histogram.compute(capture);
	ofLog() << "histogramme calculé";
}
