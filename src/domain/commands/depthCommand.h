#pragma once

#include "command.h"
#include "domain/shapes/3d/sceneModel3d.h"
#include "ofMain.h"
#include <vector>

class DepthCommand : public Command {
public:
	struct Change {
		SceneModel3d * model;
		float oldDepth;
		float newDepth;
	};

	explicit DepthCommand(std::vector<Change> changes)
		: changes(std::move(changes))
		, lastEditTime(ofGetElapsedTimef()) { }

	void undo() override {
		for (auto & c : changes) c.model->depth = c.oldDepth;
	}

	void redo() override {
		for (auto & c : changes) c.model->depth = c.newDepth;
	}

	bool tryMerge(const std::vector<Change> & next) {
		float now = ofGetElapsedTimef();
		if (now - lastEditTime > MERGE_WINDOW) return false;
		if (next.size() != changes.size()) return false;
		for (size_t i = 0; i < next.size(); ++i) {
			if (next[i].model != changes[i].model) return false;
		}
		for (size_t i = 0; i < next.size(); ++i) {
			changes[i].newDepth = next[i].newDepth;
		}
		lastEditTime = now;
		return true;
	}

private:
	std::vector<Change> changes;
	float lastEditTime;
	static constexpr float MERGE_WINDOW = 1.0f;
};
