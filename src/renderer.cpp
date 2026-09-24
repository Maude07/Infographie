// IFT3100A25_Interface/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup() {
	ofSetFrameRate(60);
	ofSetBackgroundColor(31);

	font_size = 64;

	line_offset = font_size / 2.0f;

	font.load("consolas.ttf", font_size);
}

void Renderer::update() {
	ofSetColor(stroke_color);
	ofSetLineWidth(stroke_weight);

	bounding_box = font.getStringBoundingBox(text, 0, 0);
}

void Renderer::draw() {
	ofClear(background_color);
	drawImages();

	font.drawString(
		text,
		(ofGetWidth() / 2.0f) - (bounding_box.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (bounding_box.getHeight() / 2.0f));

	ofDrawLine(
		(ofGetWidth() / 2.0f) - (bounding_box.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (bounding_box.getHeight() / 2.0f) + line_offset,
		(ofGetWidth() / 2.0f) + (bounding_box.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (bounding_box.getHeight() / 2.0f) + line_offset);
}

void Renderer::addImage(const string & path) {
	auto img = std::make_shared<SceneImage>();
	
	if (!img->image.load(path)) {
		ofLogError() << "echec du chargement de l'image: " << path;
		return;
	}
	
	float maxDimension = 400.0f;
	float w = img->image.getWidth();
	float h = img->image.getHeight();
	float scaleFactor = std::min(1.0f, maxDimension / std::max(w, h));
	img->size.set(w * scaleFactor, h * scaleFactor);
	
	float cascadeOffset = 30.0f * sceneImages.size();
	img->position.set(100 + cascadeOffset, 100 + cascadeOffset);
	
	sceneImages.push_back(img);
}

void Renderer::drawImages() {
	for (auto & img : sceneImages) {
		img->image.draw(img->position.x, img->position.y,
						img->size.x, img->size.y);
		
		if (img->selected) {
			ofNoFill();
			ofSetColor(255, 255, 0);
			ofDrawRectangle(img->getBounds());
			
			ofFill();
			ofSetColor(255, 255, 0);
			ofDrawRectangle(img->getResizeHandleBounds());
			
			ofSetColor(255);
		}
	}
}

shared_ptr<SceneImage> Renderer::hitTest(float x, float y) {
	for (auto it = sceneImages.rbegin(); it != sceneImages.rend(); ++it) {
		if ((*it)->getBounds().inside(x, y) || (*it)->getResizeHandleBounds().inside(x, y)) {
				return *it;
		}
	}
return nullptr;
}

void Renderer::deselectAll() {
	for (auto & img : sceneImages) {
		img->selected = false;
	}
}
