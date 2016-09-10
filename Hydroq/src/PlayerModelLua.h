#pragma once

#include "PlayerModel.h"


/**
* Lua wrapper for GameModel
*/
class PlayerModelLua {
	PlayerModel* playerModel;

public:
	PlayerModelLua(PlayerModel* model) : playerModel(model) {

	}

	bool IsMultiplayer() {
		return playerModel->IsMultiplayer();
	}

	string GetMap() const {
		return playerModel->GetMap();
	}


	string GetFaction() const {
		Faction fct = playerModel->GetFaction();
		return fct == Faction::BLUE ? "blue" : "red";
	}

	void SetFaction(string faction) {
		if (faction.compare("red") == 0) playerModel->SetFaction(Faction::RED);
		else playerModel->SetFaction(Faction::BLUE);
	}

	bool PlayerWin() {
		return playerModel->PlayerWin();
	}

	void SetPlayerWin(bool win) {
		playerModel->SetPlayerWin(win);
	}

	bool GameEnded() {
		return playerModel->GameEnded();
	}

	void SetGameEnded(bool gameEnded) {
		playerModel->SetGameEnded(gameEnded);
	}

	int GetUnits() {
		return playerModel->GetUnits();
	}

	void AddUnit(int num) {
		playerModel->AddUnit(num);
	}

	void RemoveUnit(int num) {
		playerModel->RemoveUnit(num);
	}

	int GetRigs() {
		return playerModel->GetRigs();
	}

	void AddRigs(int num) {
		playerModel->AddRigs(num);
	}

	void RemoveRigs(int num) {
		playerModel->RemoveRigs(num);
	}
};