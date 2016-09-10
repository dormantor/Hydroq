#pragma once

#include "ofxCogMain.h"
#include "FloatingScene.h"
#include "HydroqDef.h"
#include "PlayerModel.h"
#include "GameView.h"
#include "GameModel.h"

class AttractorPlacement : public Behavior {


public:
	FloatingScene* floatingGameScene;
	bool attractorPlaced = false;
	Node* placedAttractor = nullptr;

	HydroqAction selectedAction;
	GameView* gameView;
	GameModel* gameModel;
	int attrPlaced = 0;

	vector<Node*> attractors;

	AttractorPlacement() {

	}

	void OnInit();

	Vec2i GetTilePosition(ofVec2f objectAbsPos);

	void OnMessage(Msg& msg);

	void RefreshAttractorPosition(ofVec2f absPosition, ofVec2f scale);

	ofVec2f clickedAttrPos = ofVec2f();
	Node* clickedAttractor = nullptr;

	Node* GetClickedAttractor(ofVec2f position);

	bool RemoveAttractor(ofVec2f position);

	void InsertAttractor(ofVec2f position, Vec2i brickPos);

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		for (auto& attr : attractors) {
			attr->GetTransform().rotation += 0.4f;
		}
	}
};