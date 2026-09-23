// IFT3100A25_Interface/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup() {
	ofSetWindowTitle("interface (u)");
	//ofNoFill();
	ofFill();

	ofLog() << "<app::setup>";

	renderer.setup();

	gui.setup("interface");

	group_draw.setup("outils de dessin");

	setupPalettes();


	color_picker_background.set("couleur du canevas", ofColor(31), ofColor(0, 0), ofColor(255, 255));


	//Palette 1

	gui_color_picker_background.setup(color_picker_background);

	palette_index.set("Palette", 0, 0, allPalettes.size() - 1);
	palettesPreview[0].setup(currentPalette, color_picker_background, group_draw.getWidth());

	gui_color_picker_background.add(&palettesPreview[0]);

	group_draw.add(&gui_color_picker_background);


	//Palette 2
	gui_color_picker_stroke.setup(color_picker_stroke);

	palettesPreview[1].setup(currentPalette, color_picker_stroke, group_draw.getWidth());

	gui_color_picker_stroke.add(&palettesPreview[1]); //J'utilise la mÃªme palette partout

	slider_stroke_weight.set("largeur de la ligne", 4.0f, 0.0f, 10.0f);
	color_picker_stroke.set("couleur du trait", ofColor(255), ofColor(0, 0), ofColor(255, 255));


	group_draw.add(&gui_color_picker_stroke);


	gui.add(&group_draw);

	// image_export
	group_export.setup("exportation d'images");
	group_export.add(slider_export_fps.set("images/sec", 24, 1, 60));
	group_export.add(slider_export_duration.set("duree (s, 0=illimite)", 5.0f, 0.0f, 60.0f));
	button_record.setup("enregistrer / arreter (r)");
	button_record.addListener(this, &Application::button_record_pressed);
	group_export.add(&button_record);
	gui.add(&group_export);

	textbox.set("text", "ift3100");
	gui.add(textbox);

	button.setup("Reinitialiser");
	button.addListener(this, &Application::button_pressed);
	gui.add(&button);

	checkbox.setName("visible");
	gui.add(checkbox);

	checkbox = true;

	currentPalette = allPalettes[palette_index];

	//Ãcouter changements sur color pickers
	color_picker_background.addListener(this, &Application::onColorChanged);
	color_picker_stroke.addListener(this, &Application::onColorChanged);

	button_histogram.setup("Calculer l'histogramme");
	button_histogram.addListener(this, &Application::histogram_button_pressed);
	gui.add(&button_histogram);
}

void Application::update() {
	// assigner les Ã©tats courants de l'interface
	renderer.background_color = color_picker_background;
	renderer.stroke_color = color_picker_stroke;
	renderer.stroke_weight = slider_stroke_weight;
	renderer.text = textbox;

	currentPalette = allPalettes[0];//palette_index si on ne veut pas une palette globale

	renderer.update();
}

void Application::draw() {
	renderer.draw();

	exporter.capture();

	if (checkbox)
		gui.draw();

	// afficher l'Ã©tat de l'exportation
	if (exporter.is_recording) {
		ofSetColor(255, 0, 0);
		ofFill();
		ofDrawCircle(ofGetWidth() - 120, 30, 8);
		ofSetColor(255);
		ofDrawBitmapString("REC " + ofToString(exporter.frame_count), ofGetWidth() - 105, 35);
		ofNoFill();
	}
	if (showHistogram) {
		float guiHeight = gui.getHeight();
		renderer.drawRgbHistogram(histoR, histoG, histoB, ofRectangle(gui.getPosition().x, guiHeight + 10, gui.getWidth(), 100));
	}

}

void Application::onColorChanged(ofColor & color) {
	lastActiveColor = color;
}

void Application::setupPalettes() {

	allPalettes.resize(2);
	palettesPreview.resize(2);

	//Palette de base, on pourrait la loader d'un xml
	allPalettes[0] = {
		ofColor(15, 15, 15),
		ofColor(255, 182, 193),
		ofColor(143, 131, 216),
		ofColor(155, 184, 237)
	};

}


void Application::keyReleased(int key) {
	if (key == 117) // touche u
	{
		checkbox = !checkbox;
		ofLog() << "<toggle ui: " << checkbox << ">";
	}

	if (key == 114) {
		button_record_pressed(); // touche r
	}
}

void Application::keyPressed(int key) {

	if (key == OF_KEY_DEL || key == OF_KEY_BACKSPACE) {

		//int idx = palettesPreview[palette_index].selectedIndex;

		int active_preview_idx = -1;
		int color_idx = -1;

		for (size_t i = 0; i < palettesPreview.size(); i++) {
			if (palettesPreview[i].selectedIndex >= 0) {
				active_preview_idx = i;
				color_idx = palettesPreview[i].selectedIndex;
				break;
			}
		}

		if (color_idx >= 0 && color_idx < currentPalette.size()) {
			//Est ce qu'on veut vraiment effacer la couleur
			//Ou la rendre noir et rÃ©assignable?
			allPalettes[0].erase(allPalettes[0].begin() + color_idx);//Pour l'instant seulement la zÃ©ro car c'est une seule palette globale, sinon utiliser palette_index

			palettesPreview[palette_index].selectedIndex = -1;

			ofLog() << "<deleted color at index: " << color_idx << ">";
		}

	}

	if (key == OF_KEY_RETURN) {
		allPalettes[palette_index].push_back(lastActiveColor);

		ofLog() << "<added color: " << lastActiveColor << ">";
	}
}

void Application::button_pressed() {
	// rÃ©initialiser la zone de texte
	textbox.set("text", "ift3100");

	ofLog() << "<button pressed>";
}

void Application::button_record_pressed() {
	exporter.fps = slider_export_fps;
	exporter.duration = slider_export_duration;
	exporter.toggle();
}

void Application::windowResized(int w, int h) {
	ofLog() << "<app::windowResized: (" << w << ", " << h << ")>";
}

void Application::exit() {
	button.removeListener(this, &Application::button_pressed);
	button_record.removeListener(this, &Application::button_record_pressed);
	button_histogram.removeListener(this, &Application::histogram_button_pressed);

	ofLog() << "<app::exit>";
}

void Application::histogram_button_pressed() {
	ofImage capture;
	capture.grabScreen(gui.getWidth() + 10, 0, ofGetWidth() - gui.getWidth() - 10, ofGetHeight());

	if (!capture.isAllocated()) {
		ofLogWarning() << "histograme: capture échouée";
		return;
	}

	histoR = renderer.computeHistogram(capture, 0);
	histoG = renderer.computeHistogram(capture, 1);
	histoB = renderer.computeHistogram(capture, 2);
	showHistogram = true;
	ofLog() << "histogramme calculé";
}
