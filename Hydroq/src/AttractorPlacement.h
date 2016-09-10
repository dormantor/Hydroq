#pragma once

#include "FloatingScene.h"
#include "HydroqDef.h"
#include "PlayerModel.h"
#include "GameView.h"
#include "GameModel.h"
#include "PlayerModel.h"

/**
* Behavior that reacts on user actions and places attractors into
* the game model
*/
class AttractorPlacement : public Behavior {

private:
	// link to game scene
	FloatingScene* floatingGameScene;
	// indicator if an attractor is currently placed
	bool attractorPlaced = false;
	// link to placed attractor
	Node* placedAttractor = nullptr;
	// link to action the user selected
	HydroqAction selectedAction;
	// link to game view
	GameView* gameView;
	// link to game model
	GameModel* gameModel;
	// link to player model
	PlayerModel* playerModel;
	// number of placed attractors
	int attrPlaced = 0;
	// list of placed attractors
	vector<Node*> attractors;
	// position of clicked attractor
	ofVec2f clickedAttrPos;
	// link to clicked attractor
	Node* clickedAttractor = nullptr;
	int maxAttractors = 3;
public:
	AttractorPlacement() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

	/**
	* Refreshes position of the attractor by given desired absolute position
	*/
	void RefreshAttractorPosition(ofVec2f absPosition, ofVec2f scale);

	/**
	* Gets position of a game tile according to the absolute position
	*/
	Vec2i GetTilePosition(ofVec2f objectAbsPos);

	/**
	* Finds an attractor that is located near clicked position
	* or nullptr if there is none
	*/
	Node* GetClickedAttractor(ofVec2f position);

	/**
	* Removes attractor at selected position
	*/
	bool RemoveAttractor(ofVec2f position);

	/**
	* Inserts attractor at selected position
	* @param position absolute position of the attractor
	* @param tilePos position of a tile bound to the attractor (in game model coordinates)
	*/
	void InsertAttractor(ofVec2f position, Vec2i tilePos);

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		// rotate attractors for effect
		for (auto& attr : attractors) {
			attr->GetTransform().rotation += 0.4f;
		}
	}
};