#include "sceneModel3d.h"

namespace {
	const ofColor neutralBase(255);
	const ofColor silverBase(200, 202, 212);
	const ofColor matteSpecular(20);

	constexpr float matteShininess = 8.0f;
	constexpr float metalShininess = 64.0f;
	constexpr float metalDiffuseDarkening = 0.55f;
	constexpr float metalSpecularTint = 0.4f;

	ofColor lerpColor(const ofColor & a, const ofColor & b, float t) {
		return a.getLerped(b, ofClamp(t, 0.0f, 1.0f));
	}

	string formatFactor(float f) {
		return ofToString(f, 2);
	}
}

string ModelVariant::label() const {
	if (isOriginal()) return "original";

	vector<string> parts;
	if (stretchVertical) parts.push_back("etirement V x" + formatFactor(verticalFactor));
	if (stretchHorizontal) parts.push_back("etirement H x" + formatFactor(horizontalFactor));
	if (tinted) parts.push_back("teinte (" + ofToString((int)tint.r) + "," + ofToString((int)tint.g) + "," + ofToString((int)tint.b) + ")");
	if (metallic) parts.push_back("metallique " + ofToString((int)round(metalness * 100.0f)) + "%");

	string result;
	for (size_t i = 0; i < parts.size(); ++i) {
		if (i > 0) result += " + ";
		result += parts[i];
	}
	return result;
}

SceneModel3d::SceneModel3d(const string & id,
	shared_ptr<ofxAssimpModelLoader> m,
	const glm::vec2 & center,
	float footprint,
	const ModelVariant & v)
	: variant(v)
	, modelId(id)
	, model(move(m)) {
	name = modelId;
	size = { footprint, footprint };
	position = center - size * 0.5f;
}

ofRectangle SceneModel3d::getScreenBounds() const {
	if (!camera) return getBounds();

	const glm::vec2 c2 = position + size * 0.5f;
	const float h = min(size.x, size.y) * 0.5f;
	ofRectangle r;
	bool first = true;
	for (int i = 0; i < 8; ++i) {
		glm::vec3 corner(
			c2.x + ((i & 1) ? h : -h),
			c2.y + ((i & 2) ? h : -h),
			depth + ((i & 4) ? h : -h));
		glm::vec3 s = camera->worldToScreen(corner);
		if (first) {
			r.set(s.x, s.y, 0, 0);
			first = false;
		} else {
			r.growToInclude(s.x, s.y);
		}
	}
	return r;
}

void SceneModel3d::draw() const {
	if (!model) return;

	const glm::vec3 stretch = variant.scaleFactors();
	const glm::vec3 dims = (model->getSceneMax() - model->getSceneMin()) * stretch;
	const float largest = max({ dims.x, dims.y, dims.z });
	if (largest <= 0.0f) return;

	const glm::vec3 c = model->getSceneCenter();
	const glm::vec2 center = position + size * 0.5f;
	const float side = min(size.x, size.y);
	const float s = side / largest;

	ofColor base = neutralBase;
	if (variant.tinted)
		base = variant.tint;
	else if (variant.metallic)
		base = lerpColor(neutralBase, silverBase, variant.metalness);

	const float m = variant.metallic ? variant.metalness : 0.0f;
	const ofColor diffuse = lerpColor(base, ofColor(0), m * metalDiffuseDarkening);
	const ofColor specular = lerpColor(matteSpecular, lerpColor(ofColor(255), base, metalSpecularTint), m);
	const float shininess = ofLerp(matteShininess, metalShininess, m);

	static ofLight light;
	light.setPointLight();
	light.setPosition(center.x - side, center.y - side, depth + side * 3.0f);
	light.setDiffuseColor(ofColor(255));
	light.setSpecularColor(ofColor(255));

	static ofMaterial material;
	material.setDiffuseColor(diffuse);
	material.setAmbientColor(diffuse * 0.25f);
	material.setSpecularColor(specular);
	material.setShininess(shininess);

	const bool overrideMaterial = variant.overridesMaterial();
	if (overrideMaterial) {
		model->disableMaterials();
	}

	ofPushStyle();
	ofSetColor(overrideMaterial ? diffuse : ofColor(255));
	ofEnableLighting();
	light.enable();
	material.begin();

	ofPushMatrix();
	ofTranslate(center.x, center.y, depth);
	ofRotateYDeg(rotationY);
	ofScale(s * stretch.x, s * stretch.y, s * stretch.z);
	ofTranslate(c.x, c.y, -c.z);

	ofEnableDepthTest();
	model->drawFaces();
	ofDisableDepthTest();
	ofPopMatrix();

	material.end();
	light.disable();
	ofDisableLighting();
	ofPopStyle();

	if (overrideMaterial) {
		model->enableMaterials();
	}
}
