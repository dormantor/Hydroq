#pragma once

#include "ofxCogMain.h"
#include "GameModel.h"

class SingleGameMenu : public Behavior {
public:

	Settings mapConfig;
	string selectedMap;

	SingleGameMenu() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

	void LoadMaps();

	void ShowMapPreview(string map);

	Faction GetSelectedFaction();

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

