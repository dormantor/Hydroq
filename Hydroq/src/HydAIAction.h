#pragma once

/**
* Type of action for AI
*/
enum class HydAIActionType {
	GO_TO_EMPTY_RIG = 1,
	GO_TO_ENEMY_RIG = 2,
	CAPTURE_EMPTY_RIG = 3,
	CAPTURE_ENEMY_RIG = 4
};

/**
* AI action definition
*/
class HydAIAction {
public:
	// type of action
	HydAIActionType type;
	// index of entity the action will be applied on
	int index;

	HydAIAction() {

	}

	HydAIAction(HydAIActionType type, int index) :type(type), index(index) {

	}

	bool operator !=(const HydAIAction& rhs) { return !(*this == rhs); }

	bool operator ==(const HydAIAction& rhs) {
		return rhs.type == type && rhs.index == index;
	}
};
