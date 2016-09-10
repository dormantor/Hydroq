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
	// selected action
	HydroqAction hydroqAction;
	// number of units
	int units = 0;
	// number of captured rigs
	int rigs = 0;
	// player faction
	Faction faction;
	// indicator whether player won
	bool playerWin = false;
	// indicator whether game ended
	bool gameEnded = false;
	// selected map
	string map;
	// indicator for multiplayer
	bool isMultiplayer = false;
	// state of the network
	HydroqConnectionType connectionType;
public:

	~PlayerModel() {

	}

	void OnInit();

	/**
	* Starts a new game
	* @param faction selected faction
	* @param map selected map
	*/
	void StartGame(Faction faction, string map);

	/**
	* Starts a new game
	* @param faction selected faction
	* @param map selected map
	* @param connectionType type of the connection
	*/
	void StartGame(Faction faction, string map, HydroqConnectionType connectionType);

	/**
	* Gets indicator whether multiplayer mode is selected
	*/
	bool IsMultiplayer() const {
		return isMultiplayer;
	}

	/**
	* Gets name of selected map
	*/
	string GetMap() const {
		return map;
	}

	/**
	* Gets connection type
	*/
	HydroqConnectionType GetConnectionType() const {
		return connectionType;
	}

	/**
	* Gets type of selected action
	*/
	HydroqAction GetHydroqAction() const {
		return hydroqAction;
	}

	/**
	* Gets selected faction
	*/
	Faction GetFaction() const {
		return faction;
	}

	/**
	* Sets selected faction
	*/
	void SetFaction(Faction faction) {
		this->faction = faction;
	}

	/**
	* Gets indicator whether player won
	*/
	bool PlayerWin() const {
		return playerWin;
	}

	/**
	* Sets indicator whether player won
	*/
	void SetPlayerWin(bool win) {
		this->playerWin = win;
	}

	/**
	* Gets indicator whether game ended
	*/
	bool GameEnded() const {
		return gameEnded;
	}

	/**
	* Sets indicator whether game ended
	*/
	void SetGameEnded(bool gameEnded) {
		this->gameEnded = gameEnded;
	}

	/**
	* Sets selected faction
	* @param hydroqAction action to select
	*/
	void SetHydroqAction(HydroqAction hydroqAction);

	/**
	* Gets number of units of this player
	*/
	int GetUnits() const {
		return units;
	}

	/**
	* Adds new units 
	* @param num units to add
	*/
	void AddUnit(int num);

	/**
	* Removes units
	* @param num units to remove
	*/
	void RemoveUnit(int num);

	/**
	* Gets number of rigs of the player
	*/
	int GetRigs() const {
		return rigs;
	}

	/**
	* Adds new rigs
	* @param num number of rigs to add
	*/
	void AddRigs(int num);

	/**
	* Removes rigs
	* @param num number of rigs to remove
	*/
	void RemoveRigs(int num);

	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};