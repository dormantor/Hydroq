#include "PlayerModel.h"

void PlayerModel::OnInit() {
	hydroqAction = HydroqAction::NONE;
	units = 0;
	buildings = 0;
	faction = Faction::NONE;
	gameEnded = false;
	playerWin = false;
	isMultiplayer = false;
	networkState = HydroqNetworkState::NONE;
}

void PlayerModel::StartGame(Faction faction, string map) {
	OnInit();
	this->faction = faction;
	this->map = map;
	this->isMultiplayer = false;
}

void PlayerModel::StartGame(Faction faction, string map, HydroqNetworkState networkState) {
	OnInit();
	this->faction = faction;
	this->map = map;
	this->isMultiplayer = true;
	this->networkState = networkState;
}

void PlayerModel::SetHydroqAction(HydroqAction hydroqAction) {
	auto previous = this->hydroqAction;
	this->hydroqAction = hydroqAction;
	SendMessage(StrId(ACT_FUNC_SELECTED), spt<ValueChangeEvent<HydroqAction>>(new ValueChangeEvent<HydroqAction>(previous, hydroqAction)));
}


void PlayerModel::AddUnit(int num) {
	this->units += num;
	SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(units - num, num)));
}

void PlayerModel::RemoveUnit(int num) {
	this->units -= num;
	SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(units - num, num)));
}

void PlayerModel::AddBuildings(int num) {
	this->buildings += num;
	SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(buildings - num, num)));
}

void PlayerModel::RemoveBuilding(int num) {
	this->buildings -= num;
	SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(buildings - num, num)));
}
