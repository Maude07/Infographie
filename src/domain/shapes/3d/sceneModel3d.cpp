#include "sceneModel3d.h"
#include "presentation/celShading.h"
#include "presentation/reliefMapping.h"
#include "../../lighting/materialLibrary.h"

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

glm::vec3 SceneModel3d::worldCenter() const {
	return getCenter3d();
}

float SceneModel3d::worldBoundingRadius() const {
	const float side = min(size.x, size.y);
	const float footprint = side * 0.5f;
	if (!model) return footprint;

	const glm::vec3 stretch = variant.scaleFactors();
	const glm::vec3 dims = (model->getSceneMax() - model->getSceneMin()) * stretch;
	const float largest = max({ dims.x, dims.y, dims.z });
	if (largest <= 0.0f) return footprint;

	const float scale = side / largest;
	return glm::length(dims) * 0.5f * scale;
}

bool SceneModel3d::drawCel(const ofColor & litColor, bool replaceMaterial) const {
	ofShader * program = celShading::shader();
	if (!program || !model) return false;

	const glm::vec3 stretch = variant.scaleFactors();
	const glm::vec3 dims = (model->getSceneMax() - model->getSceneMin()) * stretch;
	const float largest = max({ dims.x, dims.y, dims.z });
	if (largest <= 0.0f) return false;

	const glm::vec3 sceneCenter = model->getSceneCenter();
	const glm::vec2 center = position + size * 0.5f;
	const float side = min(size.x, size.y);
	const float scale = side / largest;
	const glm::vec3 fallbackLight(center.x - side, center.y - side, depth + side * 3.0f);

	ofPushStyle();
	ofDisableLighting();
	ofPushMatrix();
	ofTranslate(center.x, center.y, depth);
	ofRotateYDeg(rotationY);
	ofScale(scale * stretch.x, scale * stretch.y, scale * stretch.z);
	ofEnableDepthTest();

	ofPushMatrix();
	ofTranslate(sceneCenter.x, sceneCenter.y, -sceneCenter.z);

	program->begin();
	celShading::setLightUniforms(*program, fallbackLight);
	program->setUniform1f("bands", static_cast<float>(max(celBands, 2)));
	program->setUniform1f("ambient", celAmbient);
	program->setUniform1f("outline", celOutline);

	ofPushMatrix();
	ofMultMatrix(model->getModelMatrix());

	for (unsigned int index = 0; index < model->getMeshCount(); ++index) {
		ofxAssimpMeshHelper & mesh = model->getMeshHelper(index);
		if (mesh.indices.empty()) continue;

		ofPushMatrix();
		ofMultMatrix(mesh.matrix);

		const bool hasTexture = mesh.hasTexture(aiTextureType_DIFFUSE);
		const bool hasNormals = mesh.vbo.getUsingNormals();
		const bool hasColors = mesh.mesh && mesh.mesh->HasVertexColors(0);
		const ofFloatColor albedo = replaceMaterial
			? ofFloatColor(litColor)
			: mesh.material.getDiffuseColor();

		program->setUniform4f("baseColor", albedo);
		program->setUniform1f("useTexture", hasTexture ? 1.0f : 0.0f);
		program->setUniform1f("useVertexColor", hasColors ? 1.0f : 0.0f);
		program->setUniform1f("hasNormals", hasNormals ? 1.0f : 0.0f);
		if (hasTexture) {
			program->setUniformTexture("colorTex", mesh.getTextureRef(aiTextureType_DIFFUSE), 0);
		}

		ofEnableBlendMode(mesh.blendMode);
		mesh.vbo.drawElements(GL_TRIANGLES, mesh.indices.size());
		ofPopMatrix();
	}

	ofPopMatrix();
	program->end();

	if (bShowBoundingBox) {
		const glm::vec3 boxDims = model->getSceneMax() - model->getSceneMin();
		ofPushStyle();
		ofDisableLighting();
		ofNoFill();
		ofSetColor(255, 172, 28);
		ofSetLineWidth(2.0f);
		ofDrawBox(0, 0, 0, boxDims.x, boxDims.y, boxDims.z);
		ofPopStyle();
	}

	ofPopMatrix();
	ofDisableDepthTest();
	ofPopMatrix();
	ofEnableLighting();
	ofPopStyle();
	return true;
}

bool SceneModel3d::drawRelief(const ofColor & litColor, const ofColor & specular, float shininess, bool customMaterial) const {
	ofShader * program = reliefMapping::shader();
	if (!program || !model) return false;

	bool hasTexCoords = false;
	for (unsigned int index = 0; index < model->getMeshCount(); ++index) {
		if (model->getMeshHelper(index).vbo.getUsingTexCoords()) {
			hasTexCoords = true;
			break;
		}
	}
	if (!hasTexCoords) return false;

	const glm::vec3 stretch = variant.scaleFactors();
	const glm::vec3 dims = (model->getSceneMax() - model->getSceneMin()) * stretch;
	const float largest = max({ dims.x, dims.y, dims.z });
	if (largest <= 0.0f) return false;

	const glm::vec3 sceneCenter = model->getSceneCenter();
	const glm::vec2 center = position + size * 0.5f;
	const float side = min(size.x, size.y);
	const float scale = side / largest;
	const glm::vec3 fallbackLight(center.x - side, center.y - side, depth + side * 3.0f);
	const ofFloatColor specularColor(specular);

	ofPushStyle();
	ofDisableLighting();
	ofPushMatrix();
	ofTranslate(center.x, center.y, depth);
	ofRotateYDeg(rotationY);
	ofScale(scale * stretch.x, scale * stretch.y, scale * stretch.z);
	ofEnableDepthTest();

	ofPushMatrix();
	ofTranslate(sceneCenter.x, sceneCenter.y, -sceneCenter.z);

	program->begin();
	celShading::setLightUniforms(*program, fallbackLight);

	ofPushMatrix();
	ofMultMatrix(model->getModelMatrix());

	for (unsigned int index = 0; index < model->getMeshCount(); ++index) {
		ofxAssimpMeshHelper & mesh = model->getMeshHelper(index);
		if (mesh.indices.empty()) continue;

		ofPushMatrix();
		ofMultMatrix(mesh.matrix);

		const bool hasTexture = !customMaterial && mesh.hasTexture(aiTextureType_DIFFUSE);
		const bool hasNormals = mesh.vbo.getUsingNormals();
		const bool hasNormalMap = mesh.hasTexture(aiTextureType_NORMALS);
		const ofFloatColor albedo = (customMaterial || variant.overridesMaterial())
			? ofFloatColor(litColor)
			: mesh.material.getDiffuseColor();

		reliefMapping::setMaterialUniforms(*program, albedo, specularColor, shininess, hasTexture,
			reliefColumns, reliefRows, reliefMortar);
		program->setUniform1f("hasNormals", hasNormals ? 1.0f : 0.0f);
		if (hasTexture) {
			program->setUniformTexture("colorTex", mesh.getTextureRef(aiTextureType_DIFFUSE), 0);
		}
		if (hasNormalMap) {
			program->setUniformTexture("normalTex", mesh.getTextureRef(aiTextureType_NORMALS), 1);
		}

		ofEnableBlendMode(mesh.blendMode);
		mesh.vbo.drawElements(GL_TRIANGLES, mesh.indices.size());
		ofPopMatrix();
	}

	ofPopMatrix();
	program->end();

	if (bShowBoundingBox) {
		const glm::vec3 boxDims = model->getSceneMax() - model->getSceneMin();
		ofPushStyle();
		ofDisableLighting();
		ofNoFill();
		ofSetColor(255, 172, 28);
		ofSetLineWidth(2.0f);
		ofDrawBox(0, 0, 0, boxDims.x, boxDims.y, boxDims.z);
		ofPopStyle();
	}

	ofPopMatrix();
	ofDisableDepthTest();
	ofPopMatrix();
	ofEnableLighting();
	ofPopStyle();
	return true;
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

	const bool customMaterial = materialLibrary::isCustom(materialIndex);

	static ofMaterial material;
	if (customMaterial) {
		materialLibrary::apply(material, materialLibrary::get(materialIndex));
	} else {
		material.setDiffuseColor(diffuse);
		material.setAmbientColor(diffuse * 0.25f);
		material.setSpecularColor(specular);
		material.setEmissiveColor(ofFloatColor(0.0f));
		material.setShininess(shininess);
	}


	ofColor reliefDiffuse = diffuse;
	ofColor reliefSpecular = specular;
	float reliefShininess = shininess;
	if (customMaterial) {
		const MaterialPreset & preset = materialLibrary::get(materialIndex);
		reliefDiffuse = preset.diffuse;
		reliefSpecular = preset.specular;
		reliefShininess = preset.shininess;
	}
	if (relief && drawRelief(reliefDiffuse, reliefSpecular, reliefShininess, customMaterial)) return;
	if (celShaded && drawCel(diffuse, variant.overridesMaterial())) return;

	const bool overrideMaterial = customMaterial || variant.overridesMaterial();
  
	if (overrideMaterial) {
		model->disableMaterials();
	}
	if (customMaterial) {
		model->disableTextures();
		model->disableColors();
	}

	ofPushStyle();
	ofSetColor(overrideMaterial && !customMaterial ? diffuse : ofColor(255));
	if (customMaterial) ofDisableAlphaBlending();
	ofEnableLighting();
	material.begin();

	ofPushMatrix();
	ofTranslate(center.x, center.y, depth);
	ofRotateYDeg(rotationY);
	ofScale(s * stretch.x, s * stretch.y, s * stretch.z);

	ofEnableDepthTest();

	ofPushMatrix();
	ofTranslate(c.x, c.y, -c.z);
	model->drawFaces();
	ofPopMatrix();

	if (bShowBoundingBox) {
		const glm::vec3 boxDims = model->getSceneMax() - model->getSceneMin();
		ofPushStyle();
		ofDisableLighting();
		ofNoFill();
		ofSetColor(255, 172, 28);
		ofSetLineWidth(2.0f);
		ofDrawBox(0, 0, 0, boxDims.x, boxDims.y, boxDims.z);
		ofPopStyle();
	}

	ofDisableDepthTest();
	ofPopMatrix();

	material.end();
	ofDisableLighting();
	ofPopStyle();

	if (overrideMaterial) {
		model->enableMaterials();
	}
	if (customMaterial) {
		model->enableTextures();
		model->enableColors();
	}
}
