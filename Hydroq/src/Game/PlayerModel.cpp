#include "PlayerModel.h"

void PlayerModel::OnInit() {
	hydroqAction = HydroqAction::NONE;
	units = 0;
	rigs = 0;
	faction = Faction::NONE;
	gameEnded = false;
	playerWin = false;
	isMultiplayer = false;
	connectionType = HydroqConnectionType::NONE;
}

void PlayerModel::StartGame(Faction faction, string map) {
	OnInit();
	this->faction = faction;
	this->map = map;
	this->isMultiplayer = false;
}

void PlayerModel::StartGame(Faction faction, string map, HydroqConnectionType connectionType) {
	OnInit();
	this->faction = faction;
	this->map = map;
	this->isMultiplayer = true;
	this->connectionType = connectionType;
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

void PlayerModel::AddRigs(int num) {
	this->rigs += num;
	SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(rigs - num, num)));
}

void PlayerModel::RemoveRigs(int num) {
	this->rigs -= num;
	SendMessage(StrId(ACT_COUNTER_CHANGED), spt<ValueChangeEvent<int>>(new ValueChangeEvent<int>(rigs - num, num)));
}
