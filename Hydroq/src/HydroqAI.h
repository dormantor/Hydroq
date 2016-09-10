#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "Board.h"
#include "HydroqGameModel.h"
#include "CoroutineContext.h"

#include "HydAISimulator.h"
#include "UCTAgent.h"

enum class AITaskType {
	NONE = 0, 
	CAPTURE_EMPTY = 1, 
	CAPTURE_ENEMY = 2, 
	GOTO_EMPTY = 3, 
	GOTO_ENEMY = 4
};

struct AITask {
	AITaskType type;
	vector<Vec2i> positions;
	uint64 created;
	
	AITask() : type(AITaskType::NONE), created(0) {

	}

	AITask(AITaskType type, uint64 created) : type(type), created(created) {

	}
};

struct RigInfo {
	Vec2i position;
	Vec2i nearest;
	int distance;
};

enum class AIType {
	SCRIPTED, MONTE_CARLO
};

class HydroqAI : public Behavior {

	HydroqGameModel* gameModel;
	Faction faction = Faction::NONE;
	AIType aiType;

public:
	HydroqAI(HydroqGameModel* gameModel, Faction faction, AIType aiType) :gameModel(gameModel), faction(faction),
	aiType(aiType){

	}

	void OnInit() {
		SubscribeForMessages(ACT_GAMESTATE_CHANGED);
	}

	void OnStart() {
		
	}


	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_GAMESTATE_CHANGED)) {
			auto evt = msg.GetData<GameStateChangedEvent>();
			if (evt->changeType == GameChangeType::EMPTY_RIG_CAPTURED) {
				if (evt->ownerFaction == faction) this->actualTask = AITask(); // restart task
				gameModel->DestroyAllAttractors(faction);
			}
			else if (evt->changeType == GameChangeType::ENEMY_RIG_CAPTURED) {
				if (evt->ownerFaction == faction) this->actualTask = AITask(); // restart task
				gameModel->DestroyAllAttractors(faction);
			}
		}
	}

private:

public:
	
	AITask actualTask;
	int actualTaskIndex = 0;

	vector<RigInfo> blueRedDist;
	vector<RigInfo> redBlueDist;
	vector<RigInfo> blueEmptyDist;
	vector<RigInfo> redEmptyDist;

	uint64 lastTaskTime = 0;

	virtual void Update(const uint64 delta, const uint64 absolute) {

		if (CogGetFrameCounter() % 100 == 0) {

			auto map = gameModel->GetMap();
			blueRedDist.clear();
			redBlueDist.clear();
			blueEmptyDist.clear();
			redEmptyDist.clear();
			
			vector<RigInfo>& myOpponentDist = gameModel->GetFaction() == Faction::BLUE ? redBlueDist : blueRedDist;
			vector<RigInfo>& myEmptyDist = gameModel->GetFaction() == Faction::BLUE? redEmptyDist : blueEmptyDist;
			
			CalcRigsDistance(blueRedDist, redBlueDist, blueEmptyDist, redEmptyDist);

			// if the task has been completed (there is not water at selected location), restart the task
			if (actualTask.type != AITaskType::NONE) {
				if ((actualTask.type == AITaskType::GOTO_EMPTY || actualTask.type == AITaskType::GOTO_ENEMY)){
					for (auto pos : actualTask.positions) {
						if (map->GetNode(pos)->mapNodeType != MapNodeType::WATER) {
							actualTask = AITask();
							break;
						}
					}
				}
				else {
					if ((absolute - lastTaskTime) > 10000) {
						actualTask.type = AITaskType::NONE;
					}
				}
			}

			if (actualTask.type == AITaskType::NONE) {
				switch (aiType) {
				case AIType::SCRIPTED:
					UpdateScripted(myOpponentDist, myEmptyDist, delta, absolute);
					break;
				case AIType::MONTE_CARLO:
					UpdateMonteCarlo(myOpponentDist, myEmptyDist, delta, absolute);
					break;
				}
			}
		}
	}


private:

	void UpdateMonteCarlo(vector<RigInfo>& myOpponentDist, vector<RigInfo>& myEmptyDist, uint64 delta, uint64 absolute) {
		auto aiAction = this->TrySimulator(blueRedDist, redBlueDist, blueEmptyDist, redEmptyDist);
		AITaskType selectedTaskType = (AITaskType)aiAction.type;

		if (selectedTaskType == AITaskType::CAPTURE_EMPTY) {
			Task_CaptureEmpty(myEmptyDist[aiAction.index], absolute);
		}
		else if (selectedTaskType == AITaskType::CAPTURE_ENEMY) {
			Task_CaptureEnemy(myOpponentDist[aiAction.index], absolute);
		}
		else if (selectedTaskType == AITaskType::GOTO_EMPTY) {
			auto nearestRig = myEmptyDist[aiAction.index];
			Task_Goto(nearestRig, absolute);
		}
		else if (selectedTaskType == AITaskType::GOTO_ENEMY) {
			auto nearestRig = myOpponentDist[aiAction.index];
			Task_Goto(nearestRig, absolute);
		}
	}

	void UpdateScripted(vector<RigInfo>& myOpponentDist, vector<RigInfo>& myEmptyDist, uint64 delta, uint64 absolute) {
		// capture some empty
		for (auto dist : myEmptyDist) {
			if (dist.distance == 0 && actualTask.type == AITaskType::NONE) {
				Task_CaptureEmpty(dist, absolute);
			}
		}

		// capture some opponents'
		for (auto dist : myOpponentDist) {
			if (dist.distance == 0 && actualTask.type == AITaskType::NONE) {
				Task_CaptureEnemy(dist, absolute);
			}
		}

		// build platform and go to empty rig
		if (actualTask.type == AITaskType::NONE && !myEmptyDist.empty()) {
			// get nearest empty
			sort(myEmptyDist.begin(), myEmptyDist.end(), [](const RigInfo& a, const RigInfo&b) {
				return a.distance < b.distance;
			});

			auto nearestRig = myEmptyDist[0];
			Task_Goto(nearestRig, absolute);
		}

		// build platform and go to enemy rig
		if (actualTask.type == AITaskType::NONE && !myOpponentDist.empty()) {
			// get nearest empty
			sort(myOpponentDist.begin(), myOpponentDist.end(), [](const RigInfo& a, const RigInfo&b) {
				return a.distance < b.distance;
			});

			auto nearestRig = myOpponentDist[0];
			Task_Goto(nearestRig, absolute);
		}
	}

	void CalcRigsDistance(vector<RigInfo>& blueRedDist, vector<RigInfo>& redBlueDist,
		vector<RigInfo>& blueEmptyDist, vector<RigInfo>& redEmptyDist) {

		auto blueRigs = gameModel->GetRigsByFaction(Faction::BLUE);
		auto redRigs = gameModel->GetRigsByFaction(Faction::RED);
		auto emptyRigs = gameModel->GetRigsByFaction(Faction::NONE);

		vector<Vec2i> redPos;
		vector<Vec2i> bluePos;
		vector<Vec2i> emptyPos;

		auto map = gameModel->GetMap();


		for (auto redRig : redRigs) {
			redPos.push_back(redRig->GetTransform().localPos);
		}

		for (auto blueRig : blueRigs) {
			bluePos.push_back(blueRig->GetTransform().localPos);
		}
		for (auto emptyRig : emptyRigs) {
			emptyPos.push_back(emptyRig->GetTransform().localPos);
		}

		if (!redRigs.empty() && !blueRigs.empty()) {
			// calculate distance from blue to red rigs
			for (int i = 0; i < redRigs.size(); i++) {
				CalcRigDistance(blueRigs, bluePos, redPos, i, blueRedDist);
			}

			// calculate distance from red to blue rigs
			for (int i = 0; i < blueRigs.size(); i++) {
				CalcRigDistance(redRigs, redPos, bluePos, i, redBlueDist);
			}
		}

		// calculate distance to empty rigs
		for (int i = 0; i < emptyRigs.size(); i++) {
			CalcRigDistance(blueRigs, bluePos, emptyPos, i, blueEmptyDist);
			CalcRigDistance(redRigs, redPos, emptyPos, i, redEmptyDist);
		}
	}

	void CalcRigDistance(vector<Node*>& refRigs, vector<Vec2i>& refRigsPos, vector<Vec2i>& targetRigsPos, int index, vector<RigInfo>& distances) {
		auto map = gameModel->GetMap();
		int closest = 100000;
		int closestIndex = 0;

		// find nearest opponents
		for (int j = 0; j < refRigs.size(); j++) {
			int distance = Vec2i::ManhattanDist(refRigsPos[j], targetRigsPos[index]);
			if (distance < closest) {
				closest = distance;
				closestIndex = j;
			}
		}

		// calculate real distance
		Vec2i start = Vec2i(refRigsPos[closestIndex].x - 1, refRigsPos[closestIndex].y - 1);
		Vec2i end = Vec2i(targetRigsPos[index].x - 1, targetRigsPos[index].y - 1);
		Vec2i nearest1;
		Vec2i nearest2;
		// calculate distance from A to B and from B to A
		int distance1 = map->CalcNearestReachablePosition(start, end, nearest1, closest * 2);
		int distance2 = map->CalcNearestReachablePosition(end, start, nearest2, closest * 2);


		RigInfo info;
		info.distance = min(distance1, distance2);
		info.nearest = nearest1;
		info.position = targetRigsPos[index];
		distances.push_back(info);
	}

	HydAIAction TrySimulator(vector<RigInfo>& blueRedDist, vector<RigInfo>& redBlueDist, 
		vector<RigInfo>& blueEmptyDist, vector<RigInfo>& redEmptyDist) {

		if (blueRedDist.empty() || redBlueDist.empty()) return HydAIAction();

		spt<HydAISimulator> simulator = spt<HydAISimulator>(new HydAISimulator());
		
		simulator->aiFaction = this->faction;
		HydAIState state;
		
		for (auto blueRed : blueRedDist) {
			state.distancesRed.push_back(blueRed.distance);
		}

		for (auto redBlue : redBlueDist) {
			state.distancesBlue.push_back(redBlue.distance);
		}

		for (auto blueEmpty : blueEmptyDist) {
			state.distancesBlueEmpty.push_back(blueEmpty.distance);
		}

		for (auto redEmpty : redEmptyDist) {
			state.distancesRedEmpty.push_back(redEmpty.distance);
		}

		simulator->SetActualState(state);

		auto agent = spt<UCTAgent<HydAIState, HydAIAction>>(new UCTAgent<HydAIState, HydAIAction>("UCTAgent", 20, sqrt(2), 5));
		auto action = agent->ChooseAction(simulator);
		return action;
	}

	void Task_CaptureEmpty(RigInfo dist, uint64 absolute) {
		lastTaskTime = absolute;

		cout << "jdu lajznout prazdnou" << endl;
		auto pos = Vec2i(dist.nearest.x + 1, dist.nearest.y + 1);
		actualTask = AITask(AITaskType::CAPTURE_EMPTY, absolute);
		actualTask.positions.push_back(pos);
		gameModel->AddAttractor(pos, faction, 0.3f);
	}

	void Task_CaptureEnemy(RigInfo dist, uint64 absolute) {
		lastTaskTime = absolute;

		cout << "jdu lajznout nepritele" << endl;
		auto pos = Vec2i(dist.nearest.x + 1, dist.nearest.y + 1);
		actualTask = AITask(AITaskType::CAPTURE_ENEMY, absolute);
		actualTask.positions.push_back(pos);
		gameModel->DestroyAllAttractors(faction);
		gameModel->AddAttractor(pos, faction, 0.8f);
	}

	void Task_Goto(RigInfo nearestRig, uint64 absolute) {
		lastTaskTime = absolute;

		cout << "jdu k rigu ["<< nearestRig.position.x << ", " <<nearestRig.position.y << "]" << endl;
		auto map = gameModel->GetMap();
		auto brick = map->GetNode(nearestRig.nearest);
		
		auto newTask = AITask(AITaskType::GOTO_EMPTY, absolute);

		BuildAroundBrick(nearestRig, brick, newTask, 5);

		if (!newTask.positions.empty()) actualTask = newTask;
	}

	void BuildAroundBrick(RigInfo& nearestRig, HydMapNode* brick, AITask& task, int recursiveLevels) {
		auto neighbors = brick->GetNeighborsFourDirections();

		int closerNeighborDist = 100000;
		HydMapNode* closerNeighbor = nullptr;
		for (auto neighbor : neighbors) {
			// build platform so that the rig will be close
			if (neighbor->mapNodeType == MapNodeType::WATER) {
				int distance = Vec2i::Distance(nearestRig.position, neighbor->pos);
				if (distance < closerNeighborDist) {
					closerNeighborDist = distance;
					closerNeighbor = neighbor;
				}
			}
		}

		if (closerNeighbor != nullptr) {
			if (!gameModel->PositionContainsBridgeMark(closerNeighbor->pos)) {
				task.positions.push_back(closerNeighbor->pos);
				gameModel->MarkPositionForBridge(closerNeighbor->pos, faction);
			}

			if (recursiveLevels > 0) {
				BuildAroundBrick(nearestRig, closerNeighbor, task, recursiveLevels - 1);
			}
		}
	}
};