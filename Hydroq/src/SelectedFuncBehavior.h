#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class SelectedFuncBehavior : public Behavior {
	OBJECT_PROTOTYPE(SelectedFuncBehavior)

	
	int selectedFuncSeedBed;
	int selectedFunc;

	unsigned actFuncSelected;

	void OnInit() {
		actFuncSelected = StringHash(ACT_FUNC_SELECTED);
		RegisterListening(ACT_FUNC_SELECTED);
	}

	void OnStart() {
		owner->SetRunningMode(RunningMode::INVISIBLE);
	}

	void OnMessage(Msg& msg) {
		if (msg.HasAction(StringHash(actFuncSelected))) {
			// function selected
			auto which = msg.GetDataS<ValueChangeEvent<HydroqAction>>();

			owner->SetRunningMode(RunningMode::RUNNING);

			if (which->after == HydroqAction::BUILD) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_build.png"));
			}
			else if (which->after == HydroqAction::DESTROY) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_destroy.png"));
			}
			else if (which->after == HydroqAction::FORBID) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_forbid.png"));
			}
			else if (which->after == HydroqAction::GUARD) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_guard.png"));
			}
			else if (which->after == HydroqAction::GATHER) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_gather_icon.png"));
			}
			else if (which->after == HydroqAction::ATTACK) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_attack_icon.png"));
			}
			else if (which->after == HydroqAction::NONE) {
				owner->SetRunningMode(RunningMode::INVISIBLE);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}

};