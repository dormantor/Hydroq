#include "SelectedFuncBehavior.h"
#include "Node.h"

void SelectedFuncBehavior::OnInit() {
	actFuncSelected = StrId(ACT_FUNC_SELECTED);
	SubscribeForMessages(ACT_FUNC_SELECTED);
}

void SelectedFuncBehavior::OnStart() {
	owner->SetRunningMode(RunningMode::INVISIBLE);
}

void SelectedFuncBehavior::OnMessage(Msg& msg) {
	if (msg.HasAction(StrId(actFuncSelected))) {
		// function selected
		auto which = msg.GetData<ValueChangeEvent<HydroqAction>>();

		owner->SetRunningMode(RunningMode::RUNNING);

		if (which->after == HydroqAction::BUILD) {
			owner->GetMesh<Image>()->SetImage(CogGet2DImage("game/functions_cmd_build.png"));
		}
		else if (which->after == HydroqAction::DESTROY) {
			owner->GetMesh<Image>()->SetImage(CogGet2DImage("game/functions_cmd_destroy.png"));
		}
		else if (which->after == HydroqAction::FORBID) {
			owner->GetMesh<Image>()->SetImage(CogGet2DImage("game/functions_cmd_forbid.png"));
		}
		else if (which->after == HydroqAction::ATTRACT) {
			owner->GetMesh<Image>()->SetImage(CogGet2DImage("game/functions_cmd_attract_icon.png"));
		}
		else if (which->after == HydroqAction::NONE) {
			owner->SetRunningMode(RunningMode::INVISIBLE);
		}
	}
}