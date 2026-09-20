// IFT3100A25_Interface/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup() {
	ofSetWindowTitle("interface (u)");
	ofNoFill();

	ofLog() << "<app::setup>";

	renderer.setup();

	gui.setup("interface");

	group_draw.setup("outils de dessin");

	color_picker_background.set("couleur du canevas", ofColor(31), ofColor(0, 0), ofColor(255, 255));
	color_picker_stroke.set("couleur du trait", ofColor(255), ofColor(0, 0), ofColor(255, 255));

	slider_stroke_weight.set("largeur de la ligne", 4.0f, 0.0f, 10.0f);

	group_draw.add(color_picker_background);
	group_draw.add(color_picker_stroke);
	group_draw.add(slider_stroke_weight);

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

	button.setup("button");
	button.addListener(this, &Application::button_pressed);
	gui.add(&button);

	checkbox.setName("visible");
	gui.add(checkbox);

	checkbox = true;
}

void Application::update() {
	// assigner les états courants de l'interface
	renderer.background_color = color_picker_background;
	renderer.stroke_color = color_picker_stroke;
	renderer.stroke_weight = slider_stroke_weight;
	renderer.text = textbox;

	renderer.update();
}

void Application::draw() {
	renderer.draw();

	exporter.capture();

	if (checkbox)
		gui.draw();

	// afficher l'état de l'exportation
	if (exporter.is_recording) {
		ofSetColor(255, 0, 0);
		ofFill();
		ofDrawCircle(ofGetWidth() - 120, 30, 8);
		ofSetColor(255);
		ofDrawBitmapString("REC " + ofToString(exporter.frame_count), ofGetWidth() - 105, 35);
		ofNoFill();
	}
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

void Application::button_pressed() {
	// réinitialiser la zone de texte
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

	ofLog() << "<app::exit>";
}
