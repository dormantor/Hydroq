#pragma once

#include "PlayerModel.h"
#include "Behavior.h"
#include "Definitions.h"

using namespace Cog;

/**
* Dialog that displays game end
*/
class GameEndDialog : public Behavior {	
	PlayerModel* model;
	bool firstInit = false;
public:
	GameEndDialog() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};