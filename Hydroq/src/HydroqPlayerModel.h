#pragma once

#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"


enum class HydroqAction {
	NONE, SEEDBED, BUILD, DESTROY, FORBID, GUARD, GATHER, ATTACK
};

/**
* Hydroq player model
*/
class HydroqPlayerModel : public Component {

	OBJECT(HydroqPlayerModel)

private:
	HydroqAction hydroqAction;
	

public:

	~HydroqPlayerModel() {

	}

	void Init() {

	}

	void Init(spt<ofxXml> xml) {


	}

	HydroqAction GetHydroqAction() {
		return hydroqAction;
	}

	void SetHydroqAction(HydroqAction hydroqAction) {
		auto previous = this->hydroqAction;
		this->hydroqAction = hydroqAction;
		SendMessageToListeners(StringHash(ACT_FUNC_SELECTED), 0, new ValueChangeEvent<HydroqAction>(previous, hydroqAction), nullptr);
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};