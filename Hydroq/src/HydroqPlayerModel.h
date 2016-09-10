#pragma once

#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"

enum class HydroqNetworkState {
	NONE, SERVER, CLIENT
};

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
	HydroqNetworkState networkState;
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
		networkState = HydroqNetworkState::NONE;
	}

	void StartGame(Faction faction, string map) {
		OnInit();
		this->faction = faction;
		this->map = map;
		this->isMultiplayer = false;
	}

	void StartGame(Faction faction, string map, HydroqNetworkState networkState) {
		OnInit();
		this->faction = faction;
		this->map = map;
		this->isMultiplayer = true;
		this->networkState = networkState;
	}

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

	void SetHydroqAction(HydroqAction hydroqAction) {
		auto previous = this->hydroqAction;
		this->hydroqAction = hydroqAction;
		SendMessage(StrId(ACT_FUNC_SELECTED), spt<ValueChangeEvent<HydroqAction>>(new ValueChangeEvent<HydroqAction>(previous, hydroqAction)));
	}

	int GetUnits() {
		return units;
	}

	void AddUnit(int num) {
		this->units += num;
		SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(units-num, num)));
	}

	void RemoveUnit(int num) {
		this->units -= num;
		SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(units - num, num)));
	}

	int GetBuildings() {
		return buildings;
	}

	void AddBuildings(int num) {
		this->buildings += num;
		SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(buildings - num, num)));
	}

	void RemoveBuilding(int num) {
		this->buildings -= num;
		SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(buildings - num, num)));
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};