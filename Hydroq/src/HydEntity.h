#pragma once

#include "ofxCogCommon.h"

enum class EntityType {
	BRIDGE_MARK, FORBID_MARK, DESTROY_MARK, WORKER, BRIDGE, WATER, PLATFORM, RIG, ATTRACTOR
};

class HydEntity {
public:

	HydEntity(EntityType entityType, string name) : entityType(entityType), name(name) {

	}

	EntityType entityType;
	string name;

	string GetName() {
		return name;
	}
};

class HydBridge : public HydEntity {
public:
	HydBridge() : HydEntity(EntityType::BRIDGE_MARK, "bridgemark") {

	}
};

class ForbidMark : public HydEntity {
public:
	ForbidMark() : HydEntity(EntityType::FORBID_MARK, "forbidmark") {

	}
};


class DestroyMark : public HydEntity {
public:
	DestroyMark() : HydEntity(EntityType::DESTROY_MARK, "destroymark") {

	}
};
