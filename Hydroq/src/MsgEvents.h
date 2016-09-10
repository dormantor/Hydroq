#pragma once

#include "ofxCogMain.h"
#include "Events.h"
#include "HydMap.h"
#include "GameTask.h"

enum class ObjectChangeType {
	STATIC_CHANGED = 0,
	DYNAMIC_CREATED = 1,
	DYNAMIC_REMOVED = 2,
	MOVING_CREATED = 3,
	MOVING_REMOVED = 4
};

class MapObjectChangedEvent : public MsgEvent {
public:
	ObjectChangeType changeType;
	HydMapNode* changedMapNode;
	Node* changedNode = nullptr;

	MapObjectChangedEvent(ObjectChangeType changeType, HydMapNode* changedMapNode) :
		changedMapNode(changedMapNode), changeType(changeType)
	{

	}

	MapObjectChangedEvent(ObjectChangeType changeType, HydMapNode* changedMapNode, Node* changedNode) :
		changedMapNode(changedMapNode), changeType(changeType), changedNode(changedNode)
	{

	}
};

class TaskAbortEvent : public MsgEvent {
public:
	spt<GameTask> taskToAbort;

	TaskAbortEvent(spt<GameTask> taskToAbort) :
		taskToAbort(taskToAbort)
	{

	}
};