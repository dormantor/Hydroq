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
	HydMapNode* changedNode;
	spt<HydEntity> changedEntity;

	MapObjectChangedEvent(ObjectChangeType changeType, HydMapNode* changedNode) :
		changedNode(changedNode), changeType(changeType)
	{
		changedEntity = spt<HydEntity>();
	}

	MapObjectChangedEvent(ObjectChangeType changeType, HydMapNode* changedNode, spt<HydEntity> changedEntity) :
		changedNode(changedNode), changeType(changeType), changedEntity(changedEntity)
	{

	}
};