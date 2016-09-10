#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"
#include "Board.h"
#include "HydroqGameModel.h"
#include "CoroutineContext.h"

enum class AITaskType {
	NONE, CAPTURE_EMPTY, CAPTURE_ENEMY, GOTO_EMPTY, GOTO_ENEMY
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

class HydroqAI : public Behavior {
	OBJECT_PROTOTYPE(HydroqAI)

	HydroqGameModel* gameModel;
	Faction faction = Faction::NONE;

	void OnInit() {
		RegisterListening(ACT_GAMESTATE_CHANGED);
		gameModel = GETCOMPONENT(HydroqGameModel);
	}

	void OnStart() {
		
	}


	void OnMessage(Msg& msg) {
		if (msg.HasAction(ACT_GAMESTATE_CHANGED)) {
			auto evt = msg.GetDataS<GameStateChangedEvent>();
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

	virtual void Update(const uint64 delta, const uint64 absolute) {

		// todo: continuous update
		this->faction = gameModel->GetFaction() == Faction::BLUE ? Faction::RED : Faction::BLUE;

		if (CogGetFrameCounter() % 100 == 0) {

			auto map = gameModel->GetMap();

			vector<RigInfo> blueRedDist;
			vector<RigInfo> redBlueDist;
			vector<RigInfo> blueEmptyDist;
			vector<RigInfo> redEmptyDist;

			vector<RigInfo>& myOpponentDist = gameModel->GetFaction() == Faction::BLUE ? redBlueDist : blueRedDist;
			vector<RigInfo>& myEmptyDist = gameModel->GetFaction() == Faction::BLUE? redEmptyDist : blueEmptyDist;
			
			CalcRigsDistance(blueRedDist, redBlueDist, blueEmptyDist, redEmptyDist);

			for (auto i : blueRedDist) {
				cout << "BLUE_RED " << i.distance << ", [" << i.nearest.x << ", " << i.nearest.y << "]" << endl;
			}

			for (auto i : redBlueDist) {
				cout << "RED_BLUE " << i.distance << ", [" << i.nearest.x << ", " << i.nearest.y << "]" << endl;
			}

			for (auto i : blueEmptyDist) {
				cout << "BLUE_EMPTY " << i.distance << ", [" << i.nearest.x << ", " << i.nearest.y << "]" << endl;
			}

			for (auto i : redEmptyDist) {
				cout << "RED_EMPTY " << i.distance << ", [" << i.nearest.x << ", " << i.nearest.y << "]" << endl;
			}


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
			}

			// capture some empty
			for (auto dist : myEmptyDist) {
				if (dist.distance == 0 && actualTask.type == AITaskType::NONE) {
					auto pos = Vec2i(dist.nearest.x + 1, dist.nearest.y + 1);
					actualTask = AITask(AITaskType::CAPTURE_EMPTY, absolute);
					actualTask.positions.push_back(pos);
					gameModel->AddAttractor(pos, faction, 0.3f);
				}
			}

			// capture some opponents'
			for (auto dist : myOpponentDist) {
				if (dist.distance == 0 && actualTask.type == AITaskType::NONE) {
					auto pos = Vec2i(dist.nearest.x + 1, dist.nearest.y + 1);
					actualTask = AITask(AITaskType::CAPTURE_ENEMY, absolute);
					actualTask.positions.push_back(pos);
					gameModel->DestroyAllAttractors(faction);
					gameModel->AddAttractor(pos, faction, 0.8f);
				}
			}

			// build platform and go to empty rig
			if (actualTask.type == AITaskType::NONE && !myEmptyDist.empty()) {
				// get nearest empty
				sort(myEmptyDist.begin(), myEmptyDist.end(), [](const RigInfo& a, const RigInfo&b) {
					return a.distance < b.distance;
				});


				auto nearestRig = myEmptyDist[0];
				auto brick = map->GetNode(nearestRig.nearest);
				auto neighbors = brick->GetNeighbors();

				auto newTask = AITask(AITaskType::GOTO_EMPTY, absolute);

				for (auto neighbor : neighbors) {
					// build platform so that the rig will be close
					if (neighbor->mapNodeType == MapNodeType::WATER && !gameModel->PositionContainsBridgeMark(neighbor->pos) &&
						Vec2i::Distance(nearestRig.position, neighbor->pos) <= Vec2i::Distance(nearestRig.position, nearestRig.nearest)) {
						newTask.positions.push_back(neighbor->pos);
						gameModel->MarkPositionForBridge(neighbor->pos, faction);
					}
				}

				if (!newTask.positions.empty()) actualTask = newTask;
			}

			// build platform and go to enemy rig
			if (actualTask.type == AITaskType::NONE && !myOpponentDist.empty()) {
				// get nearest empty
				sort(myOpponentDist.begin(), myOpponentDist.end(), [](const RigInfo& a, const RigInfo&b) {
					return a.distance < b.distance;
				});

				auto nearestRig = myOpponentDist[0];
				auto brick = map->GetNode(nearestRig.nearest);
				auto neighbors = brick->GetNeighbors();

				auto newTask = AITask(AITaskType::GOTO_ENEMY, absolute);

				for (auto neighbor : neighbors) {
					// build platform so that the rig will be close
					if (neighbor->mapNodeType == MapNodeType::WATER && !gameModel->PositionContainsBridgeMark(neighbor->pos) &&
						Vec2i::Distance(nearestRig.position, neighbor->pos) <= Vec2i::Distance(nearestRig.position, nearestRig.nearest)) {
						newTask.positions.push_back(neighbor->pos);
						gameModel->MarkPositionForBridge(neighbor->pos, faction);
					}
				}

				if (!newTask.positions.empty()) actualTask = newTask;

			}
		}
	}


private:

	void CalcRigsDistance(vector<RigInfo>& blueRedDist, vector<RigInfo>& redBlueDist,
		vector<RigInfo>& blueEmptyDist, vector<RigInfo>& redEmptyDist) {

		auto blueRigs = gameModel->GetRigsByFaction(Faction::BLUE);
		auto redRigs = gameModel->GetRigsByFaction(Faction::RED);
		auto emptyRigs = gameModel->GetRigsByFaction(Faction::NONE);

		vector<Vec2i> redPos = vector<Vec2i>();
		vector<Vec2i> bluePos = vector<Vec2i>();
		vector<Vec2i> emptyPos = vector<Vec2i>();

		auto gameModel = GETCOMPONENT(HydroqGameModel);
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

		// find nearest blue
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
		Vec2i nearest;
		int distance = map->CalcNearestReachablePosition(start, end, nearest, closest * 2);

		RigInfo info;
		info.distance = distance;
		info.nearest = nearest;
		info.position = targetRigsPos[index];
		distances.push_back(info);
	}
};