#pragma once

/**
* Type of action for AI
*/
enum class HydAIActionType {
	CAPTURE_EMPTY = 1,		/** capture empty rig */
	CAPTURE_ENEMY = 2,		/** capture enemy rig */
	GOTO_EMPTY = 3,			/** go closer to empty rig */
	GOTO_ENEMY = 4			/** go closer to enemy rig */
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
