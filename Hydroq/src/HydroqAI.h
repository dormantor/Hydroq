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
	Vec2i position;
	uint64 created;
	
	AITask() : type(AITaskType::NONE), position(Vec2i(0)), created(0) {

	}

	AITask(AITaskType type, Vec2i position, uint64 created) : type(type), created(created), position(position) {

	}
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
			}
			else if (evt->changeType == GameChangeType::ENEMY_RIG_CAPTURED) {
				if (evt->ownerFaction == faction) this->actualTask = AITask(); // restart task
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

			vector<pair<int, Vec2i>> blueRedDist;
			vector<pair<int, Vec2i>> redBlueDist;
			vector<pair<int, Vec2i>> blueEmptyDist;
			vector<pair<int, Vec2i>> redEmptyDist;

			vector<pair<int, Vec2i>>& myOpponentDist = gameModel->GetFaction() == Faction::BLUE ? redBlueDist : blueRedDist;
			vector<pair<int, Vec2i>>& myEmptyDist = gameModel->GetFaction() == Faction::BLUE? redEmptyDist : blueEmptyDist;
			
			CalcRigsDistance(blueRedDist, redBlueDist, blueEmptyDist, redEmptyDist);

			for (auto i : blueRedDist) {
				cout << "BLUE_RED " << i.first << ", [" << i.second.x << ", " << i.second.y << "]" << endl;
			}

			for (auto i : redBlueDist) {
				cout << "RED_BLUE " << i.first << ", [" << i.second.x << ", " << i.second.y << "]" << endl;
			}

			for (auto i : blueEmptyDist) {
				cout << "BLUE_EMPTY " << i.first << ", [" << i.second.x << ", " << i.second.y << "]" << endl;
			}

			for (auto i : redEmptyDist) {
				cout << "RED_EMPTY " << i.first << ", [" << i.second.x << ", " << i.second.y << "]" << endl;
			}

			// capture some empty
			for (auto dist : myEmptyDist) {
				if (dist.first == 0 && actualTask.type == AITaskType::NONE) {
					auto pos = Vec2i(dist.second.x + 1, dist.second.y + 1);
					actualTask = AITask(AITaskType::GOTO_EMPTY, pos, absolute);
					gameModel->AddAttractor(pos, faction, 0.3f);
				}
			}

			for (auto dist : myOpponentDist) {
				if (dist.first == 0 && actualTask.type == AITaskType::NONE) {
					auto pos = Vec2i(dist.second.x + 1, dist.second.y + 1);
					actualTask = AITask(AITaskType::GOTO_EMPTY, pos, absolute);
					gameModel->DestroyAllAttractors(faction);
					gameModel->AddAttractor(pos, faction, 0.8f);
				}
			}
		}
	}


private:


	void CalcRigsDistance(vector<pair<int, Vec2i>>& blueRedDist, vector<pair<int, Vec2i>>& redBlueDist, 
		vector<pair<int, Vec2i>>& blueEmptyDist, vector<pair<int, Vec2i>>& redEmptyDist) {

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

	void CalcRigDistance(vector<Node*>& refRigs, vector<Vec2i>& refRigsPos, vector<Vec2i>& targetRigsPos, int index, vector<pair<int, Vec2i>>& distances) {
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

		distances.push_back(make_pair(distance, nearest));
	}
};