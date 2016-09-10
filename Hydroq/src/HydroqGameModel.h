#pragma once

#include "Component.h"
#include "ofxSQLite.h"
#include "HydroqDef.h"
#include "Events.h"


enum class RightPanelFunc {
	NONE, SEEDBED, BUILD, DESTROY, FORBID, GUARD
};

/**
* Hydroq game model
*/
class HydroqGameModel : public Component {

	OBJECT(HydroqGameModel)

private:
	RightPanelFunc rightPanelFunc;

public:

	~HydroqGameModel() {

	}

	void Init() {

	}

	void Init(spt<ofxXml> xml) {


	}

	RightPanelFunc GetRightPanelFunc() {
		return rightPanelFunc;
	}

	void SetRightPanelFunc(RightPanelFunc rightPanelFunc) {
		auto previous = this->rightPanelFunc;
		this->rightPanelFunc = rightPanelFunc;
		SendMessageNoBubbling(StringHash(ACT_FUNC_SELECTED), 0, new ValueChangeEvent<RightPanelFunc>(previous, rightPanelFunc), nullptr);
	}
};