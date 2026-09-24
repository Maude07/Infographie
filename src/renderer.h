// IFT3100A25_Interface/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

struct SceneImage {
	ofImage image;
	ofVec2f position;
	ofVec2f size;
	bool selected = false;
	
	static constexpr float handleSize = 10.0f;
	
	ofRectangle getBounds() const {
		return ofRectangle(position.x, position.y, size.x, size.y);
	}
	
	ofRectangle getResizeHandleBounds() const {
		ofVec2f corner = position + size;
		return ofRectangle(corner.x - handleSize / 2.0f,
						   corner.y - handleSize / 2.0f,
						   handleSize, handleSize);
	}
};

class Renderer {
public:
	ofTrueTypeFont font;

	ofColor background_color;

	ofColor stroke_color;

	ofRectangle bounding_box;

	string text;

	float stroke_weight;

	float line_offset;

	int font_size;

	vector<shared_ptr<SceneImage>> sceneImages;

	void addImage(const string & path);
	void drawImages();
	
	shared_ptr<SceneImage> hitTest(float x, float y);
	void deselectAll();

	void setup();
	void update();
	void draw();

};
