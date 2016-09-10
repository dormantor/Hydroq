#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class SelectedFuncBehavior : public Behavior {
	OBJECT_PROTOTYPE(SelectedFuncBehavior)

	
	int selectedFuncSeedBed;
	int selectedFunc;

	int actFuncSelected;

	void Init() {
		owner->SetRunningMode(RunningMode::INVISIBLE);
		actFuncSelected = StringHash(ACT_FUNC_SELECTED);

		RegisterListening(owner->GetScene(), ACT_FUNC_SELECTED);
	}


	void OnMessage(Msg& msg) {
		if (msg.GetAction() == actFuncSelected) {
			// function selected
			auto which = msg.GetDataS<ValueChangeEvent<HydroqAction>>();

			owner->SetRunningMode(RunningMode::RUNNING);

			if (which->after == HydroqAction::SEEDBED) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_build_seedbed.png"));
			}
			else if (which->after == HydroqAction::BUILD) {
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
			else if (which->after == HydroqAction::NONE) {
				owner->SetRunningMode(RunningMode::INVISIBLE);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}

};