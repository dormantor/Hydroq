#pragma once

#include "HydroqDef.h"
#include "GameBoard.h"
#include "GameModel.h"
#include "CoroutineContext.h"

#include "HydAISimulator.h"
#include "UCTAgent.h"

/**
* Struct describing selected task by AI
*/
struct AITask {
	// type of task
	HydAIActionType type;
	// positions involved
	vector<Vec2i> positions;
	// time of creation
	uint64 created;
	// indicator, if the task is completed
	bool isCompleted = true;
	// indicator whether the task was restarted
	bool isRestarted = false;
	
	AITask() : type(HydAIActionType::GOTO_EMPTY), created(0), isCompleted(true) {

	}

	AITask(HydAIActionType type, uint64 created) : type(type), created(created), isCompleted(false) {

	}
};

/**
* Structure describing a rig
*/
struct RigInfo {
	// position of rig
	Vec2i position;
	// position of nearest another rig
	Vec2i nearest;
	// distance to nearest rig
	int distance;
};

/**
* Behavior for artificial intelligence, uses Monte-Carlo tree search
* and abstraction of the game map for selection the best action
*/
class GameAI : public Behavior {
	// link to game model
	GameModel* gameModel;
	// faction of the AI player
	Faction faction = Faction::NONE;

	AITask actualTask;
	// distances to red rigs from blue faction
	vector<RigInfo> blueRedDist;
	// distances to blue rigs from red faction
	vector<RigInfo> redBlueDist;
	// distances to empty rigs from blue faction
	vector<RigInfo> blueEmptyDist;
	// distances to empty rigs from red faction
	vector<RigInfo> redEmptyDist;
	// the last time a task has been assigned
	uint64 lastTaskTime = 0;
	
public:
	GameAI(GameModel* gameModel, Faction faction) 
		: gameModel(gameModel), faction(faction){

	}

	void OnInit() {
		SubscribeForMessages(ACT_GAMESTATE_CHANGED);
	}

	void OnStart() {
		
	}

	void OnMessage(Msg& msg);

	virtual void Update(const uint64 delta, const uint64 absolute);

protected:
	/**
	* Updates actual task by running the MonteCarlo tree search algorithm
	* @param myOpponentDist distances to opponent rigs
	* @param myEmptyDist distances to empty rigs from the perspective of AI faction
	* @param delta delta time since the last iteration
	* @param absolute absolute time
	*/
	void UpdateMonteCarlo(vector<RigInfo>& myOpponentDist, vector<RigInfo>& myEmptyDist, uint64 delta, uint64 absolute);

	/**
	* Calculates distances to rigs, using manhattan distance and A* path finding algorithm
	*/
	void CalcRigsDistance();

	/**
	* Calculates nearest distance to target rig
	* @param refRigs all rigs the distance can be measured from
	* @param refRigsPos positions of all rigs the distance can be measured from
	* @param targetRigsPos positions of rigs to calculate
	* @param index index of targetRigsPos collection, selecting the rig that will be calculated
	* @param distances output collection with distances to rig
	*/
	void CalcRigDistance(vector<Node*>& refRigs, vector<Vec2i>& refRigsPos, vector<Vec2i>& targetRigsPos, int index, vector<RigInfo>& distances);

	/**
	* Tries MonteCarlo tree search that selects a possible action
	*/
	HydAIAction TrySimulator();

	/**
	* Executes the action which means capturing an empty rig
	*/
	void Task_CaptureEmpty(RigInfo dist, uint64 absolute);

	/**
	* Executes the action which means capturing an enemy rig
	*/
	void Task_CaptureEnemy(RigInfo dist, uint64 absolute);

	/**
	* Executes the action which means going to another rig (building bridge)
	*/
	void Task_Goto(RigInfo nearestRig, uint64 absolute);

	/**
	* Recursively marks positions to build the path
	* @param nearestRig nearest rig to which should the way lead
	* @param tile actual map tile
	* @param task selected task
	* @param recursiveLevels number of recursive calls that should be made
	*/
	void BuildAroundTile(RigInfo& nearestRig, GameMapTile* tile, AITask& task, int recursiveLevels);
};