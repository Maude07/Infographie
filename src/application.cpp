// IFT3100A25_Interface/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup() {
	ofSetWindowTitle("interface (u)");
	ofFill();

	ofLog() << "<app::setup>";

	renderer.setup();

	gui.setup("interface");

	group_draw.setup("outils de dessin");

	setupPalettes();

	color_picker_background.set("couleur du canevas", ofColor(31), ofColor(0, 0), ofColor(255, 255));

	button_import_image.setup("Importer une image");
	button_import_image.addListener(this, &Application::button_import_pressed);
	gui.add(&button_import_image);

	//Palette 1
	gui_color_picker_background.setup(color_picker_background);

	palette_index.set("Palette", 0, 0, allPalettes.size() - 1);
	palettesPreview[0].setup(currentPalette, color_picker_background, group_draw.getWidth());

	gui_color_picker_background.add(&palettesPreview[0]);

	group_draw.add(&gui_color_picker_background);

	//Palette 2
	gui_color_picker_stroke.setup(color_picker_stroke);

	palettesPreview[1].setup(currentPalette, color_picker_stroke, group_draw.getWidth());

	gui_color_picker_stroke.add(&palettesPreview[1]);

	slider_stroke_weight.set("largeur de la ligne", 4.0f, 0.0f, 10.0f);
	color_picker_stroke.set("couleur du trait", ofColor(255), ofColor(0, 0), ofColor(255, 255));

	group_draw.add(&gui_color_picker_stroke);

	gui.add(&group_draw);

	textbox.set("text", "ift3100");
	gui.add(textbox);

	button.setup("Reinitialiser");
	button.addListener(this, &Application::button_pressed);
	gui.add(&button);

	checkbox.setName("visible");
	gui.add(checkbox);

	checkbox = true;

	currentPalette = allPalettes[palette_index];

	color_picker_background.addListener(this, &Application::onColorChanged);
	color_picker_stroke.addListener(this, &Application::onColorChanged);
}

void Application::update() {
	renderer.background_color = color_picker_background;
	renderer.stroke_color = color_picker_stroke;
	renderer.stroke_weight = slider_stroke_weight;
	renderer.text = textbox;

	currentPalette = allPalettes[0];

	renderer.update();
}

void Application::draw() {
	renderer.draw();

	if (checkbox)
		gui.draw();
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
	if (key == 117)
	{
		checkbox = !checkbox;
		ofLog() << "<toggle ui: " << checkbox << ">";
	}
}

void Application::keyPressed(int key) {
	if (key == OF_KEY_DEL || key == OF_KEY_BACKSPACE) {
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
			allPalettes[0].erase(allPalettes[0].begin() + color_idx);

			palettesPreview[palette_index].selectedIndex = -1;

			ofLog() << "<deleted color at index: " << color_idx << ">";
		}
	}

	if (key == OF_KEY_RETURN) {
		allPalettes[palette_index].push_back(lastActiveColor);

		ofLog() << "<added color: " << lastActiveColor << ">";
	}
}

void Application::mousePressed(int x, int y, int button) {
	renderer.deselectAll();
	
	selectedImage = renderer.hitTest(x, y);
	if (!selectedImage) return;
	
	selectedImage->selected = true;
	isResizing = selectedImage->getResizeHandleBounds().inside(x, y);
	
	if (!isResizing) {
		dragOffset.set(x - selectedImage->position.x, y - selectedImage->position.y);
	}
}
	
void Application::mouseDragged(int x, int y, int button) {
	if (!selectedImage) return;
	
	if (isResizing) {
		selectedImage->size.set(
								std::max(20.0f, x - selectedImage->position.x),
								std::max(20.0f, y - selectedImage->position.y));
	} else {
		selectedImage->position.set(x - dragOffset.x, y - dragOffset.y);
	}
}

void Application::mouseReleased(int x, int y, int button) {
	isResizing = false;
}

void Application::button_pressed() {
	textbox.set("text", "ift3100");
	
	ofLog() << "<button pressed>";
}

void Application::button_import_pressed() {
	ofFileDialogResult result = ofSystemLoadDialog("Choisir une image");
	if (result.bSuccess) {
		renderer.addImage(result.getPath());
	}
}

void Application::windowResized(int w, int h) {
	ofLog() << "<app::windowResized: (" << w << ", " << h << ")>";
}

void Application::exit() {
	button.removeListener(this, &Application::button_pressed);
	button_import_image.removeListener(this, &Application::button_import_pressed);
	
	ofLog() << "<app::exit>";
}
