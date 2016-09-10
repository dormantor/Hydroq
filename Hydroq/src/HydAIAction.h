#pragma once

/**
* Type of action for AI
*/
enum class HydAIActionType {
	CAPTURE_EMPTY = 1,
	CAPTURE_ENEMY = 2,
	GOTO_EMPTY = 3,
	GOTO_ENEMY = 4
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
