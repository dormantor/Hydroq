#pragma once

#include "ofxCogMain.h"
#include "Events.h"
#include "HydMap.h"

enum class ObjectChangeType {
	STATIC_CHANGED,
	DYNAMIC_CHANGED,
	DYNAMIC_REMOVED
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