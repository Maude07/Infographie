// IFT3100A25_Interface/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup() {
	ofSetFrameRate(60);
	ofSetBackgroundColor(31);

	// paramètre
	font_size = 64;

	// variable
	line_offset = font_size / 2.0f;

	// importer une police de caractères
	font.load("consolas.ttf", font_size);
}

void Renderer::update() {
	// paramètres des outils de dessin
	ofSetColor(stroke_color);
	ofSetLineWidth(stroke_weight);

	// résolution du rectangle qui délimite la zone de texte
	bounding_box = font.getStringBoundingBox(text, 0, 0);
}

void Renderer::draw() {
	// couleur d'arrière-plan
	ofClear(background_color);

	// dessiner le texte au centre de la fenêtre d'affichage
	font.drawString(
		text,
		(ofGetWidth() / 2.0f) - (bounding_box.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (bounding_box.getHeight() / 2.0f));

	// dessiner une ligne sous le texte
	ofDrawLine(
		(ofGetWidth() / 2.0f) - (bounding_box.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (bounding_box.getHeight() / 2.0f) + line_offset,
		(ofGetWidth() / 2.0f) + (bounding_box.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (bounding_box.getHeight() / 2.0f) + line_offset);
}

std::vector<int> Renderer::computeHistogram(const ofImage& image, int channel) {
	const ofPixels & pixels = image.getPixels();

	std::vector<int> hist(256, 0);
	int width = pixels.getWidth();
	int height = pixels.getHeight();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			ofColor c = pixels.getColor(x, y);

			unsigned char value;
			switch (channel) {
			case 0:
				value = c.r;
				break;
			case 1:
				value = c.g;
				break;
			case 2:
				value = c.b;
				break;
			case 3:
				value = c.a;
				break;
			default:
				value = c.r;
				break;
			}
			hist[value]++;
		}
	}
	return hist;
}
