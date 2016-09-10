#pragma once

#include "Component.h"
#include "HydroqDef.h"

using namespace Cog;

/**
* Type of game task
*/
enum class GameTaskType {
	BRIDGE_BUILD,		/** task for building bridge */
	BRIDGE_DESTROY,		/** task for destroying the bridge */
	ATTRACT				/** task for comming to the attractor position */
};

/**
* Crate describing game task to do
*/
class GameTask {
private:
	// task type
	GameTaskType type;
	// indicator, if the task is still processing
	bool isProcessing = false;
	// indicator, if the task ended
	bool isEnded = false;
	// node referred by task
	Node* taskNode = nullptr;
	// node that serves the task
	Node* handlerNode = nullptr;
	// indicator whether the task should be recalculated
	bool needRecalculation = false;
	// indicator whether the task was delayed (because the target wasn't reachable at the moment)
	bool isDelayed = false;
	// indicator whether the task has been reserved
	bool isReserved = false;
	// collection of nodes that have this task reserved
	vector<Node*> reservedNodes;
	// the time the task was reserved
	uint64 reservedTime = 0;
	// faction concerned
	Faction faction;

public:

	GameTask(GameTaskType taskType, Faction faction) :type(taskType), faction(faction) {

	}

	/**
	* Gets task type
	*/
	GameTaskType GetType() const {
		return type;
	}

	/**
	* Sets task type
	*/
	void SetType(GameTaskType type) {
		this->type = type;
	}

	/**
	* Gets indicator whether the task is still processing
	*/
	bool IsProcessing() const {
		return isProcessing;
	}

	/**
	* Sets indicator whether the task is still processing
	*/
	void SetIsProcessing(bool processing) {
		this->isProcessing = processing;
	}

	/**
	* Gets indicator, if the task ended
	*/
	bool IsEnded() const {
		return isEnded;
	}

	/**
	* Sets indicator, if the task ended
	*/
	void SetIsEnded(bool isEnded) {
		this->isEnded = isEnded;
	}

	/**
	* Gets node referred by task
	*/
	Node* GetTaskNode() const {
		return taskNode;
	}

	/**
	* Sets node referred by task
	*/
	void SetTaskNode(Node* taskNode) {
		this->taskNode = taskNode;
	}

	/**
	* Gets node that serves the task
	*/
	Node* GetHandlerNode() const {
		return handlerNode;
	}

	/**
	* Sets node that serves the task
	*/
	void SetHandlerNode(Node* handlerNode) {
		this->handlerNode = handlerNode;
	}

	/**
	* Gets indicator whether the task should be recalculated
	*/
	bool NeedRecalculation() const {
		return needRecalculation;
	}

	/**
	* Sets indicator whether the task should be recalculated
	*/
	void SetNeedRecalculation(bool needRecalculation) {
		this->needRecalculation = needRecalculation;
	}

	/**
	* Gets indicator whether the task was delayed (because the target wasn't reachable at the moment)
	*/
	bool IsDelayed() const {
		return isDelayed;
	}

	/**
	* Sets indicator whether the task was delayed
	*/
	void SetIsDelayed(bool isDelayed) {
		this->isDelayed = isDelayed;
	}

	/**
	* Gets indicator whether the task has been reserved
	*/
	bool IsReserved() const {
		return isReserved;
	}

	/**
	* Sets indicator whether the task has been reserved
	*/
	void SetIsReserved(bool isReserved) {
		this->isReserved = isReserved;
	}

	/**
	* Gets collection of nodes that have this task reserved
	*/
	vector<Node*>& GetReservedNodes() {
		return this->reservedNodes;
	}

	/**
	* Gets the time the task was reserved
	*/
	uint64 GetReservedTime() const {
		return reservedTime;
	}

	/**
	* Sets the time the task was reserved
	*/
	void SetReservedTime(uint64 reservedTime) {
		this->reservedTime = reservedTime;
	}

	/**
	* Gets the concerned faction
	*/
	Faction GetFaction() const {
		return faction;
	}

	/**
	* Sets the concerned faction
	*/
	void SetFaction(Faction faction) {
		this->faction = faction;
	}

	/**
	* Returns true, if selected node is in the collection of reservers
	*/
	bool IsNodeReserved(int nodeId);

	/**
	* Removes selected node from the collection of reserved nodes
	*/
	void RemoveReserverNode(int nodeId);
};