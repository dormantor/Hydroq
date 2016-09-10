#pragma once

#include "Component.h"
#include "HydroqDef.h"

using namespace Cog;

class GameTask {
public:
	StringHash taskSh;
	bool isProcessing = false;
	bool isEnded = false;
	Node* taskNode = nullptr;
	Node* handlerNode = nullptr;

	GameTask(StringHash taskSh) :taskSh(taskSh) {

	}
};