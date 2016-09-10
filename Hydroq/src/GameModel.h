#pragma once

#include "Behavior.h"
#include "HydroqDef.h"
#include "MsgPayloads.h"
#include "GameMap.h"
#include "MsgEvents.h"
#include "StateMachine.h"
#include "GameTask.h"
#include "Scene.h"
#include "NodeCellObject.h"
#include "PlayerModel.h"
#include "Rig.h"

/**
* Hydroq game model
*/
class GameModel : public Behavior {

private:
	// map of static objects (map, water, platforms)
	GameMap* hydroqMap;
	// cell partitioner for moving objects
	GridSpace<NodeCellObject>* cellSpace;
	// dynamic objects (building marks, forbidden marks)
	map<Vec2i, Node*> dynObjects;
	// moving objects (workers)
	vector<Node*> movingObjects;
	// placed attractors
	map<Faction, map<Vec2i, Node*>> attractors;
	// rig entities
	map<Vec2i, Rig> rigs;
	
	// game scene that runs separately from the stage
	Scene* gameScene;
	// root node of the game scene
	Node* rootNode;
	// name of selected map
	string mapName;
	// list of waiting tasks
	vector<spt<GameTask>> gameTasks;
	// link to player model
	PlayerModel* playerModel;

public:

	~GameModel() {

	}

	void OnInit();

	/**
	* Gets root node of the scene that belongs to the behavior and
	* is updated separately from the Stage
	*/
	Node* GetRootNode() const {
		return rootNode;
	}

	/**
	* Gets map
	*/
	GameMap* GetMap() const {
		return hydroqMap;
	}

	/**
	* Gets partitioned map space for quick search
	*/
	GridSpace<NodeCellObject>* GetCellSpace() const {
		return cellSpace;
	}

	/**
	* Gets name of actual map
	*/
	string GetMapName() const {
		return mapName;
	}


	/**
	* Returns true, if a building can be built on selected position
	*/
	bool IsPositionFreeForBuilding(Vec2i position);

	/**
	* Returns true, if a bridge can be built on selected position
	*/
	bool IsPositionFreeForBridge(Vec2i position);

	/**
	* Returns true, if a bridge mark is placed on selected position
	*/
	bool PositionContainsBridgeMark(Vec2i position);
	
	/**
	* Marks position for a new bridge
	*/
	void MarkPositionForBridge(Vec2i position, Faction faction);

	/**
	* Deletes mark for building a bridge
	*/
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
	* Deletes forbidden mark
	*/
	void DeleteForbidMark(Vec2i position);

	/**
	* Returns true, if the position can be destroyed
	*/
	bool IsPositionFreeForDestroy(Vec2i position);

	/**
	* Returns true, if the position is marked for destroy
	*/
	bool PositionContainsDestroyMark(Vec2i position);

	/**
	* Marks selected position to be destroyed
	*/
	void MarkPositionForDestroy(Vec2i position, Faction faction);

	/**
	* Creates a new worker at selected position
	*/
	void SpawnWorker(ofVec2f position, Vec2i rigPosition);

	/**
	* Creates a new worker at selected position for selected faction
	*/
	void SpawnWorker(ofVec2f position, Faction faction, int identifier, Vec2i rigPosition);

	/**
	* Creates a platform on selected position
	*/
	void BuildPlatform(Vec2i position);

	/**
	* Creates a platform on selected position for selected faction
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
	* Adds a new attractor at selected position
	*/
	void AddAttractor(Vec2i position, Faction faction, float cardinality);

	/**
	* Destroys selected attractor
	*/
	void DestroyAttractor(Vec2i position, Faction faction);

	/**
	* Destroys all attractors of selected faction
	*/
	void DestroyAllAttractors(Faction faction);

	/**
	* Changes cardinality of an attractor at selected position
	*/
	void ChangeAttractorCardinality(Vec2i position, Faction faction, float cardinality);

	/**
	* Calculates cardinality of attractor with selected id
	*/
	float CalcAttractorAbsCardinality(Faction faction, int attractorId);

	/**
	* Changes owner of a rig
	*/
	void ChangeRigOwner(Node* rig, Faction faction);

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
	* Gets collection of rigs
	*/
	map<Vec2i, Rig>& GetRigs() {
		return rigs;
	}

	/**
	* Gets rig by position
	*/
	Rig& GetRigAtPosition(Vec2i pos)  {
		return rigs[pos];
	}

	/**
	* Gets collection of placed attractors
	*/
	map<Faction, map<Vec2i, Node*>>& GetAttractors() {
		return attractors;
	}

	/**
	* Gets copy of game tasks
	*/
	void GetGameTasksByFaction(Faction faction, vector<spt<GameTask>>& output);

	/**
	* Gets collection of moving objects by type
	*/
	void GetMovingObjectsByType(EntityType type, Faction faction, vector<Node*>& output);

	/**
	* Removes game task
	*/
	bool RemoveGameTask(spt<GameTask> task);

	/**
	* Finds nearest rig around selected position by its faction
	*/
	Node* FindNearestRigByFaction(Faction fact, ofVec2f startPos);

	/**
	* Gets collection of rigs by faction
	*/
	void GetRigsByFaction(Faction fact, vector<Node*>& output);

	/**
	* Gets collection of attractors by faction
	*/
	void GetAttractorsByFaction(Faction fact, vector<Node*>& output);


	virtual void Update(const uint64 delta, const uint64 absolute);

private:

	/**
	* Returns true, if the map at selected position is of a given type
	*/
	bool IsPositionOfType(Vec2i position, EntityType type);

	/**
	* Creates dynamic object at selected position
	*/
	Node* CreateDynamicObject(Vec2i position, EntityType entityType, Faction faction, int identifier);

	/**
	* Destroys dynamic object at selected position
	*/
	void DestroyDynamicObject(Vec2i position);

	/**
	* Creates moving object at selected position
	*/
	Node* CreateMovingObject(ofVec2f position, EntityType entityType, Faction faction, int identifier);

	/**
	* Sends message outside the game scene (to the stage)
	*/
	void SendMessageOutside(StrId action, int subaction, spt<MsgPayload> data);

	/**
	* Sends message to the game scene that runs separately from the stage
	*/
	void SendMessageToModel(StrId action, int subaction, spt<MsgPayload> data);

	/**
	* Creates a new node of a given type
	*/
	Node* CreateNode(EntityType entityType, ofVec2f position, Faction faction, int identifier);

	/**
	* Divides rigs into factions
	*/
	void DivideRigsIntoFactions();
};