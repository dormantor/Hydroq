#pragma once

#include "Component.h"
#include "HydroqDef.h"

using namespace Cog;

class GameTask {
public:
	// StringHash of the task
	StringHash taskSh;
	// indicator, if the task is still processing
	bool isProcessing = false;
	// inficator, if the task ended
	bool isEnded = false;
	// node referred by task
	Node* taskNode = nullptr;
	// node that serves the task
	Node* handlerNode = nullptr;

	GameTask(StringHash taskSh) :taskSh(taskSh) {

	}
};