#pragma once

#include "ofxCogMain.h"
#include "PlayerModel.h"


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

	bool locked = false;

	void OnMessage(Msg& msg);

	void UnselectNode(int nodeId);

	void SelectNode(int nodeId);

	void SelectCommandBuild() {
		playerModel->SetHydroqAction(HydroqAction::BUILD);
	}

	void SelectCommandDestroy() {
		playerModel->SetHydroqAction(HydroqAction::DESTROY);
	}

	void SelectCommandForbid() {
		playerModel->SetHydroqAction(HydroqAction::FORBID);
	}

	void SelectCommandAttract() {
		playerModel->SetHydroqAction(HydroqAction::ATTRACT);
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};