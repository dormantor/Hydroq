#pragma once

#include "ofxCogMain.h"

enum class EntityType {
	BRIDGE_MARK, FORBID_MARK, DESTROY_MARK, WORKER, BRIDGE, WATER, PLATFORM, RIG, ATTRACTOR
};

class GameEntity {
public:

	GameEntity(EntityType entityType, string name) : entityType(entityType), name(name) {

	}

	EntityType entityType;
	string name;

	string GetName() {
		return name;
	}
};

class HydBridge : public GameEntity {
public:
	HydBridge() : GameEntity(EntityType::BRIDGE_MARK, "bridgemark") {

	}
};

class ForbidMark : public GameEntity {
public:
	ForbidMark() : GameEntity(EntityType::FORBID_MARK, "forbidmark") {

	}
};


class DestroyMark : public GameEntity {
public:
	DestroyMark() : GameEntity(EntityType::DESTROY_MARK, "destroymark") {

	}
};
