#pragma once

#include "Behavior.h"
#include "Settings.h"
#include "HydroqDef.h"

using namespace Cog;

/**
* Behavior for single game menu
*/
class SingleGameMenu : public Behavior {
private:
	Settings mapConfig;
	string selectedMap;

public:
	SingleGameMenu() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

	/**
	* Loads all maps into view
	*/
	void LoadMaps();

	/**
	* Shows preview of selected map
	*/
	void ShowMapPreview(string map);

	Faction GetSelectedFaction();

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

