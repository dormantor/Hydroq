#pragma once

#include "ofxCogMain.h"
#include "PlayerModel.h"
#include "HydroqDef.h"
#include "GameBoard.h"

class GameModel;

class LeftPanel : public Behavior {

	spt<TransformEnt> originTrans;
	spt<TransformEnt> animTrans;
	PlayerModel* playerModel;
	GameModel* gameModel = nullptr;
public:
	LeftPanel() {

	}

	void OnInit();

	void OnStart();

	void OnMessage(Msg& msg);

private:

	Node* counterUnits;
	Node* counterBuildings;

	void RefreshCounters();

	Node* mapIcon = nullptr;

	void ReloadMapIcon(string pathToFile);

	Node* mapBorder = nullptr;
	Node* gameBoard = nullptr;

	void ReloadMapBorder();

	void HandleMapDetailHit(spt<InputEvent> evt);

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};