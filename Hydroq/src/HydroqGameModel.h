#pragma once

#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"
#include "MsgEvents.h"
#include "StateMachine.h"
#include "GameTask.h"
#include "Scene.h"
#include "CellPartitioner.h"
#include "HydroqPlayerModel.h"

enum class Faction {
	RED, BLUE
};

/**
* Hydroq game model
*/
class HydroqGameModel : public Component {

	OBJECT(HydroqGameModel)

private:
	// static objects (map, water, platforms)
	HydMap* hydroqMap;
	// cell partitioner for moving objects
	CellSpace* cellSpace;
	
	// dynamic objects (buildings)
	map<Vec2i, Node*> dynObjects;
	// moving objects (units)
	vector<Node*> movingObjects;
	
	// scene and its root node that run separately from the view
	// part of the game
	Scene* gameScene;
	Node* rootNode;

	Faction faction;
	string mapName;
	bool multiplayer = false;
	// list of waiting tasks
	vector<spt<GameTask>> gameTasks;
	
	HydroqPlayerModel* playerModel;

public:

	~HydroqGameModel() {

	}

	void Init();

	void Init(spt<ofxXml> xml) {
		Init();
	}

	void InitModel(Faction faction, string map, bool isMultiplayer);

	Node* GetRootNode() {
		return rootNode;
	}

	HydMap* GetMap() {
		return hydroqMap;
	}

	CellSpace* GetCellSpace() {
		return cellSpace;
	}

	Faction GetFaction() {
		return faction;
	}

	string GetMapName() {
		return mapName;
	}

	bool IsMultiplayer() {
		return multiplayer;
	}


	/**
	* Returns true, if a building can be built on selected position
	*/
	bool IsPositionFreeForBuilding(Vec2i position);

	/**
	* Returns true, if a bridge (platform) can be built on selected position
	*/
	bool IsPositionFreeForBridge(Vec2i position);

	/**
	* Returns true, if a bridge mark is placed on selected position
	*/
	bool PositionContainsBridgeMark(Vec2i position);
	
	/**
	* Marks position for a new bridge
	*/
	void MarkPositionForBridge(Vec2i position);

	void DeleteBridgeMark(Vec2i position);

	/**
	* Returns true, if the position can be forbidden
	*/
	bool IsPositionFreeForForbid(Vec2i position);

	/**
	* Returns true, if the selected position is forbidden
	*/
	bool PositionContainsForbidMark(Vec2i position);

	/**
	* Marks forbidden position
	*/
	void MarkPositionForForbid(Vec2i position);

	/**
	* Delete forbidden mark
	*/
	void DeleteForbidMark(Vec2i position);

	/**
	* Returns true, if a guard mark can be placed on selected position
	*/
	bool IsPositionFreeForGuard(Vec2i position);

	/**
	* Returns true, if the position is marked for guard
	*/
	bool PositionContainsGuardMark(Vec2i position);

	/**
	* Marks position that will be guarded
	*/
	void MarkPositionForGuard(Vec2i position);

	/**
	* Deletes guard mark at selected position
	*/
	void DeleteGuardMark(Vec2i position);

	/**
	* Returns true, if the position can be destroyed
	*/
	bool IsPositionFreeForDestroy(Vec2i position);

	/**
	* Returns true, if the position is marked for destroy
	*/
	bool PositionContainsDestroyMark(Vec2i position);

	/**
	* Marks selected position to be intended for destroy
	*/
	void MarkPositionForDestroy(Vec2i position);

	/**
	* Creates a new worker at selected position
	*/
	void SpawnWorker(ofVec2f position);

	/**
	* Creates a new worker at selected position for selected faction
	*/
	void SpawnWorker(ofVec2f position, Faction faction, int identifier);


	void CreateSeedBed(Vec2i position);
	

	void CreateSeedBed(Vec2i position, Faction faction, int identifier);

	/**
	* Creates a platform on selected position
	*/
	void BuildPlatform(Vec2i position);

	/**
	* Creates a platform on selected position
	*/
	void BuildPlatform(Vec2i position, Faction faction, int identifier);

	/**
	* Destroys a platform on selected position
	*/
	void DestroyPlatform(Vec2i position);

	/**
	* Destroys a platform on selected position
	*/
	void DestroyPlatform(Vec2i position, Faction faction, int identifier);


	/**
	* Gets collection of dynamic objects
	*/
	map<Vec2i, Node*>& GetDynamicObjects() {
		return this->dynObjects;
	}

	/**
	* Gets collection of moving objects
	*/
	vector<Node*>& GetMovingObjects() {
		return this->movingObjects;
	}

	/**
	* Gets collection of game tasks
	*/
	vector<spt<GameTask>>& GetGameTasks() {
		return this->gameTasks;
	}

	/**
	* Gets copy of game tasks
	*/
	vector<spt<GameTask>> GetGameTaskCopy();

	vector<Node*> GetMovingObjectsByType(EntityType type, Faction faction);

	bool RemoveGameTask(spt<GameTask> task);

	Node* FindNearestDynamicNode(EntityType type, ofVec2f startPos);

	virtual void Update(const uint64 delta, const uint64 absolute);

	private:

	bool IsPositionOfType(Vec2i position, EntityType type);

	Node* CreateDynamicObject(Vec2i position, EntityType entityType, Faction faction, int identifier);

	/**
	* Destroys dynamic object at selected position
	*/
	void DestroyDynamicObject(Vec2i position);

	Node* CreateMovingObject(ofVec2f position, EntityType entityType, Faction faction, int identifier);

	void SendMessageOutside(StringHash action, int subaction, MsgEvent* data);

	void SendMessageToModel(StringHash action, int subaction, MsgEvent* data);

	Node* CreateNode(EntityType entityType, ofVec2f position, Faction faction, int identifier);
};