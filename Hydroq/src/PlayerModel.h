#pragma once

#include "Component.h"
#include "HydroqDef.h"
#include "MsgPayloads.h"
#include "GameMap.h"


/**
* Hydroq player model
*/
class PlayerModel : public Component {

private:
	HydroqAction hydroqAction;
	int units = 0;
	int buildings = 0;
	Faction faction;
	bool playerWin = false;
	bool gameEnded = false;
	string map;
	bool isMultiplayer = false;
	HydroqNetworkState networkState;
public:

	~PlayerModel() {

	}

	void OnInit();

	void StartGame(Faction faction, string map);

	void StartGame(Faction faction, string map, HydroqNetworkState networkState);

	bool IsMultiplayer() {
		return isMultiplayer;
	}

	string GetMap() {
		return map;
	}

	HydroqNetworkState GetNetworkState() {
		return networkState;
	}

	HydroqAction GetHydroqAction() {
		return hydroqAction;
	}

	Faction GetFaction() {
		return faction;
	}

	bool PlayerWin() {
		return playerWin;
	}

	void SetPlayerWin(bool win) {
		this->playerWin = win;
	}

	bool GameEnded() {
		return gameEnded;
	}

	void SetGameEnded(bool gameEnded) {
		this->gameEnded = gameEnded;
	}

	void SetFaction(Faction faction) {
		this->faction = faction;
	}

	void SetHydroqAction(HydroqAction hydroqAction);

	int GetUnits() {
		return units;
	}

	void AddUnit(int num);

	void RemoveUnit(int num);

	int GetBuildings() {
		return buildings;
	}

	void AddBuildings(int num);

	void RemoveBuilding(int num);

	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};