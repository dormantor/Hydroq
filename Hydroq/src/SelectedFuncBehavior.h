#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "HydroqGameModel.h"

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
			auto which = msg.GetDataS<ValueChangeEvent<RightPanelFunc>>();

			owner->SetRunningMode(RunningMode::RUNNING);

			if (which->after == RightPanelFunc::SEEDBED) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_build_seedbed.png"));
			}
			else if (which->after == RightPanelFunc::BUILD) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_build.png"));
			}
			else if (which->after == RightPanelFunc::DESTROY) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_destroy.png"));
			}
			else if (which->after == RightPanelFunc::FORBID) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_forbid.png"));
			}
			else if (which->after == RightPanelFunc::GUARD) {
				owner->GetShape<spt<Image>>()->SetImage(CogGet2DImage("game/functions_cmd_guard.png"));
			}
			else if (which->after == RightPanelFunc::NONE) {
				owner->SetRunningMode(RunningMode::INVISIBLE);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}

};