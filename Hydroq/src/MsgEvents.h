#pragma once

#include "ofxCogMain.h"
#include "Events.h"
#include "HydMap.h"
#include "GameTask.h"

enum class ObjectChangeType {
	STATIC_CHANGED,
	DYNAMIC_CREATED,
	DYNAMIC_REMOVED,
	MOVING_CREATED,
	MOVING_REMOVED
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