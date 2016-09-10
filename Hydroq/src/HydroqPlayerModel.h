#pragma once

#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"

enum class HydroqAction {
	NONE, BUILD, DESTROY, FORBID, ATTRACT
};

/**
* Hydroq player model
*/
class HydroqPlayerModel : public Component {

private:
	HydroqAction hydroqAction;
	int units = 0;
	int buildings = 0;
	Faction faction;
	bool playerWin = false;
	bool gameEnded = false;
	string map;
	bool isMultiplayer = false;
public:

	~HydroqPlayerModel() {

	}

	void OnInit() {
		hydroqAction = HydroqAction::NONE;
		units = 0;
		buildings = 0;
		faction = Faction::NONE;
		gameEnded = false;
		playerWin = false;
		isMultiplayer = false;
	}

	void StartGame(Faction faction, string map, bool isMultiplayer) {
		OnInit();
		this->faction = faction;
		this->map = map;
		this->isMultiplayer = isMultiplayer;
	}

	bool IsMultiplayer() {
		return isMultiplayer;
	}

	string GetMap() {
		return map;
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

	void SetHydroqAction(HydroqAction hydroqAction) {
		auto previous = this->hydroqAction;
		this->hydroqAction = hydroqAction;
		SendMessageToListeners(StrId(ACT_FUNC_SELECTED), 0, new ValueChangeEvent<HydroqAction>(previous, hydroqAction), nullptr);
	}

	int GetUnits() {
		return units;
	}

	void AddUnit(int num) {
		this->units += num;
		SendMessageToListeners(StrId(ACT_COUNTER_CHANGED), 0, new ValueChangeEvent<int>(units-num, num), nullptr);
	}

	void RemoveUnit(int num) {
		this->units -= num;
		SendMessageToListeners(StrId(ACT_COUNTER_CHANGED), 0, new ValueChangeEvent<int>(units - num, num), nullptr);
	}

	int GetBuildings() {
		return buildings;
	}

	void AddBuildings(int num) {
		this->buildings += num;
		SendMessageToListeners(StrId(ACT_COUNTER_CHANGED), 0, new ValueChangeEvent<int>(buildings - num, num), nullptr);
	}

	void RemoveBuilding(int num) {
		this->buildings -= num;
		SendMessageToListeners(StrId(ACT_COUNTER_CHANGED), 0, new ValueChangeEvent<int>(buildings - num, num), nullptr);
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};