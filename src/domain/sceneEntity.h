#pragma once

#include "ofMain.h"

using namespace std;

class SceneObject;

class SceneEntity {

public:
	SceneEntity(string name = "Entity", SceneObject * obj = nullptr)
		: linkedObject(obj) {

		nameTextBox.set("", name);

		parameters.setName(name);
		parameters.add(uiPosition.set("Position", glm::vec3(0), glm::vec3(-500), glm::vec3(500)));
		parameters.add(entityColor.set("Color", ofColor::darkGrey, ofColor::black, ofColor::white));

		rowRectangle.width = width;
		rowRectangle.height = height;
		rowRectangle.setPosition(glm::vec3(0, 0, 0));
	}

	virtual ~SceneEntity() {
	}

	virtual void update() {
	}

	virtual void drawRow() {
		ofPushMatrix();
		ofTranslate(uiPosition);

		if (isSelected) {
			ofSetColor(ofColor::lightBlue);
		}
		else if (isMouseInside(ofGetMouseX(), ofGetMouseY()))
		{
			ofSetColor(ofColor::lightGrey);
		}
		else {
			ofSetColor(entityColor);
		}



		ofDrawRectangle(rowRectangle);

		ofSetColor(ofColor::white);
		ofDrawBitmapString(nameTextBox.get(), 10, 20);

		ofPopMatrix();
	}

	ofParameterGroup & getParameters() {
		return parameters;
	}

	ofParameter<string> & getNameRow() {
		return nameTextBox;
	}

	void setName(const string & name) {
		nameTextBox.set(name);
		parameters.setName(name);
	}
	bool isMouseInside(int mouseX, int mouseY) {
		float localMouseX = mouseX - uiPosition.get().x;
		float localMouseY = mouseY - uiPosition.get().y;
		return rowRectangle.inside(localMouseX, localMouseY);
	}
	void setPosition(const glm::vec3 newPosition) {
		uiPosition.set(newPosition);
	}

	void setIsSelected(bool isSelected) {
		this->isSelected = isSelected;
	}

	bool getIsSelected() {
		return this->isSelected;
	}


	SceneObject * getLinkedObject() { return linkedObject; }

private:

	SceneObject * linkedObject = nullptr;
	vector<shared_ptr<SceneEntity>> children;

	ofParameterGroup parameters;
	ofParameter<glm::vec3> uiPosition;
	ofParameter<ofColor> entityColor;
	ofParameter<string> nameTextBox;

	ofRectangle rowRectangle;

	bool isSelected = false;

	int width = 200;
	int height = 30;
};
