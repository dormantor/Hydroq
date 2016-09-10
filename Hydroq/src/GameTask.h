#pragma once

#include "Component.h"
#include "HydroqDef.h"

using namespace Cog;

enum class GameTaskType {
	BRIDGE_BUILD, BRIDGE_DESTROY, ATTRACT
};

class GameTask {
public:
	// task type
	GameTaskType type;
	// indicator, if the task is still processing
	bool isProcessing = false;
	// inficator, if the task ended
	bool isEnded = false;
	// node referred by task
	Node* taskNode = nullptr;
	// node that serves the task
	Node* handlerNode = nullptr;

	bool isDelayed = false;
	// indicator, if the task is reserved
	bool isReserved = false;
	// node that reserved the task
	vector<Node*> reserverNodes;
	uint64 reserverTime = 0;

	bool IsNodeReserved(int nodeId);

	void RemoveReserverNode(int nodeId);
	GameTask(GameTaskType taskType) :type(taskType) {

	}
};