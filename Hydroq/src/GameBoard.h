#pragma once

#include "ofxCogMain.h"

class GameBoard : public Behavior {
	
private:
	ResourceCache* cache;
	Settings mapConfig;

public:

	GameBoard() {

	}

	void OnInit();

	void ZoomIntoPositionCenter(ofVec2f positionRelative);

	void SetNewPosition(Trans& transform, ofVec3f& newAbsPos);

	void CheckNewPosition(Trans& transform, ofVec3f& newPos);

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};