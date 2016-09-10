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
	int units = 0;
	int buildings = 0;

public:

	~HydroqPlayerModel() {

	}

	void Init() {

	}

	void Init(spt<ofxXml> xml) {
		Init();
	}

	HydroqAction GetHydroqAction() {
		return hydroqAction;
	}

	void SetHydroqAction(HydroqAction hydroqAction) {
		auto previous = this->hydroqAction;
		this->hydroqAction = hydroqAction;
		SendMessageToListeners(StringHash(ACT_FUNC_SELECTED), 0, new ValueChangeEvent<HydroqAction>(previous, hydroqAction), nullptr);
	}

	int GetUnits() {
		return units;
	}

	void AddUnit(int num) {
		this->units += num;
		SendMessageToListeners(StringHash(ACT_COUNTER_CHANGED), 0, new ValueChangeEvent<int>(units-num, num), nullptr);
	}

	int GetBuildings() {
		return buildings;
	}

	void AddBuildings(int num) {
		this->buildings += num;
		SendMessageToListeners(StringHash(ACT_COUNTER_CHANGED), 0, new ValueChangeEvent<int>(buildings - num, num), nullptr);
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};