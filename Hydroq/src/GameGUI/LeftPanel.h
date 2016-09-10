#pragma once

#include "PlayerModel.h"
#include "HydroqDef.h"
#include "GameBoard.h"

using namespace Cog;

class GameModel;

/**
* Behavior that controls the left panel that displays map and number of units
*/
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

	// link to node that displays number of units
	Node* counterUnits;
	// link to node that displays number of buildings
	Node* counterBuildings;

	/**
	* Refreshes number of units and rigs
	*/
	void RefreshCounters();

	// link to node that displays map icon
	Node* mapIcon = nullptr;

	/**
	* Reloads map icon 
	*/
	void ReloadMapIcon(string pathToFile);

	/**
	* Reloads map border so that it will displays actually visible part of the game board
	*/
	void ReloadMapBorder();


	Node* mapBorder = nullptr;
	Node* gameBoard = nullptr;

	/**
	* Handles hit on map and zooms the game board to the selected location
	*/
	void HandleMapDetailHit(spt<InputEvent> evt);

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};