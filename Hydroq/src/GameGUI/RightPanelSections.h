#pragma once

#include "Behavior.h"
#include "PlayerModel.h"

using namespace Cog;

/**
* Behavior that reacts to clicked icons on the right panel
* and sets currently selected action
*/
class RightPanelSections : public Behavior {

	PlayerModel* playerModel;

	int nodeCommandBuildId;
	int nodeCommandDestroyId;
	int nodeCommandForbidId;
	int nodeCommandAttractId;

	int selectedNodeId = -1;

public:

	RightPanelSections() {

	}

	void OnInit();

	// indicator for message locking, ensures that
	// no other message will be received during processing another 
	bool locked = false;

	void OnMessage(Msg& msg);

	void SelectNode(int nodeId);

	void UnselectNode(int nodeId);

	/**
	* Selects building command as an actual command
	*/
	void SelectCommandBuild() {
		playerModel->SetHydroqAction(HydroqAction::BUILD);
	}

	/**
	* Selects destroy command as an actual command
	*/
	void SelectCommandDestroy() {
		playerModel->SetHydroqAction(HydroqAction::DESTROY);
	}

	/**
	* Selects forbid command as an actual command
	*/
	void SelectCommandForbid() {
		playerModel->SetHydroqAction(HydroqAction::FORBID);
	}

	/**
	* Selects attractor command as an actual command
	*/
	void SelectCommandAttract() {
		playerModel->SetHydroqAction(HydroqAction::ATTRACT);
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};