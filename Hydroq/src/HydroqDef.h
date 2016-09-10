#pragma once

/**
* Type of game entity
*/
enum class EntityType {
	BRIDGE_MARK,	/** mark for building the bridge */
	FORBID_MARK,	/** mark for forbidden area */
	DESTROY_MARK,	/** mark for destroying the bridge */
	WORKER,			/** worker unit */
	BRIDGE,			/** bridge */
	WATER,			/** water */
	PLATFORM,		/** platform (nearest area around the rig) */
	RIG,			/** drilling rig */
	ATTRACTOR		/** attractor */
};

/** Type of faction */
enum class Faction {
	NONE = 1,
	RED = 2,
	BLUE = 3
};

/** Type of network connection */
enum class HydroqConnectionType {
	NONE,	/** undefined */
	SERVER,	/** listening peer */
	CLIENT	/** connected peer*/
};

/** 
* Type of user action
*/
enum class HydroqAction {
	NONE,		/** undefined */
	BUILD,		/** build bridge */
	DESTROY,	/** destroy bridge*/
	FORBID,		/** forbid area */
	ATTRACT		/** attract to area*/
};

// actions
#define ACT_BRICK_CLICKED "BRICK_CLICKED"
#define ACT_FUNC_SELECTED "FUNC_SELECTED"
#define ACT_COUNTER_CHANGED "COUNTER_CHANGED"
#define ACT_MAP_OBJECT_CHANGED "MAP_OBJECT_CHANGED"
#define ACT_TASK_ABORTED "TASK_ABORTED"
#define ACT_SERVER_FOUND "SERVER_FOUND"
#define ACT_SYNC_OBJECT_CHANGED "SYNC_OBJECT_CHANGED"
#define ACT_GAMESTATE_CHANGED "GAMESTATE_CHANGED"

// attributes
#define ATTR_SEEDBED_FREQUENCY "SEEDBED_FREQUENCY"
#define ATTR_SEEDBED_LASTSPAWN "SEEDBED_LAST_SPAWN"
#define ATTR_SERVER_IP "SERVER_IP"
#define ATTR_FACTION "FACTION"
#define ATTR_ENTITYTYPE "ENTITY_TYPE"
#define ATTR_MAP "MAP"
#define ATTR_BRICK_POS "BRICK_POS"
#define ATTR_CARDINALITY "CARDINALITY"
#define ATTR_RIGENTITY "RIGENTITY"

// states
#define STATE_WORKER_IDLE "WORKER_IDLE"
#define STATE_WORKER_GOTOBRIDGE "WORKER_GOTO_BRIDGE"
#define STATE_WORKER_BUILD "WORKER_BUILD"
#define STATE_WORKER_ATTRACTOR_FOLLOW "ATTRACTOR_FOLLOW"

// goals
#define GOAL_GOTO_POSITION "GOTO_POSITION"
#define GOAL_BUILD_BRIDGE "BUILD_BRIDGE"
#define GOAL_DESTROY_BRIDGE "DESTROY_BRIDGE"

// application id for communication
#define HYDROQ_APPID 1006
// port of the connected peer
#define HYDROQ_CLIENTPORT 16895
// port of the listening peer
#define HYDROQ_SERVERPORT 16896

// net messages
#define NET_MULTIPLAYER_INIT "MULTIPLAYER_INIT" // server initialization
#define NET_MULTIPLAYER_COMMAND "MULTIPLAYER_COMMAND" // user command
