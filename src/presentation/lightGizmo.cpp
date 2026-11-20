#include "presentation/lightGizmo.h"

namespace {
	constexpr float radius = 8.0f;
	constexpr float selectedRadius = 14.0f;
	constexpr float targetRadius = 4.0f;
	const ofColor disabledColor(90);
}

namespace lightGizmo {
	void draw(const LightRig & rig, int selectedIndex) {
		ofPushStyle();
		ofDisableLighting();
		ofDisableDepthTest();
		ofSetLineWidth(1.0f);

		for (size_t i = 0; i < rig.lights.size(); ++i) {
			const SceneLight & light = rig.lights[i];
			if (light.type == LightType::Ambient) continue;

			const bool selected = static_cast<int>(i) == selectedIndex;
			ofSetColor(light.enabled ? light.color : disabledColor);

			ofFill();
			ofDrawSphere(light.position, selected ? selectedRadius : radius);

			if (light.type == LightType::Directional || light.type == LightType::Spot) {
				ofDrawLine(light.position, light.target);
				ofNoFill();
				ofDrawSphere(light.target, targetRadius);
			}
		}
		ofPopStyle();
	}
}
