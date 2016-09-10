#pragma once

#include "ofxCogCommon.h"

enum class EntityType {
	SEEDBED, BRIDGE_MARK, FORBID_MARK, GUARD_MARK, DESTROY_MARK, WORKER
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

class SeebedBuilding : public HydEntity {
public:
	SeebedBuilding() : HydEntity(EntityType::SEEDBED, "seedbed") {

	}

};

class ForbidMark : public HydEntity {
public:
	ForbidMark() : HydEntity(EntityType::FORBID_MARK, "forbidmark") {

	}
};

class GuardMark : public HydEntity {
public:
	GuardMark() : HydEntity(EntityType::GUARD_MARK, "guardmark") {

	}
};

class DestroyMark : public HydEntity {
public:
	DestroyMark() : HydEntity(EntityType::DESTROY_MARK, "destroymark") {

	}
};
