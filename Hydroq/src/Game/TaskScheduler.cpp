#include "TaskScheduler.h"


void TaskScheduler::CalcAssignedTasks(vector<spt<GameTask>>& tasks, map<int, int>& output) {
	
	for (auto& task : tasks) {
		if (task->IsReserved()) {
			for (auto node : task->GetReservedNodes()) {
				if (output.find(node->GetId()) == output.end()) {
					output[node->GetId()] = 0;
				}
				else {
					output[node->GetId()] += 1;
				}
			}
		}
	}
}

void TaskScheduler::ScheduleTasks(uint64 absolute) {
	if (playerModel->IsMultiplayer()) {
		ScheduleTasksForFaction(absolute, playerModel->GetFaction());
	}
	else {
		ScheduleTasksForFaction(absolute, Faction::BLUE);
		ScheduleTasksForFaction(absolute, Faction::RED);
	}
}

void TaskScheduler::ScheduleTasksForFaction(uint64 absolute, Faction faction) {
	vector<spt<GameTask>> allTasks;
	gameModel->GetGameTasksByFaction(faction, allTasks);
	
	vector<Node*> allWorkers;
	gameModel->GetMovingObjectsByType(EntityType::WORKER, faction, allWorkers);
	auto map = gameModel->GetMap();

	std::map<int, int> assignedTasks;
	CalcAssignedTasks(allTasks, assignedTasks);

	COGMEASURE_BEGIN("HYDROQ_SCHEDULER");
	
	for (auto& task : allTasks) {

		if (!task->IsDelayed() && !task->IsEnded() && !task->IsProcessing() &&
			(!task->IsReserved() || (task->IsReserved() && (absolute - task->GetReservedTime()) > 10000))
			&& (task->GetType() == GameTaskType::BRIDGE_BUILD || task->GetType() == GameTaskType::BRIDGE_DESTROY 
				|| task->GetType() == GameTaskType::ATTRACT)) {

			auto taskLocation = task->GetTaskNode()->GetTransform().localPos;

			// node at position the bridge will stay
			auto mapNode = map->GetTile((int)taskLocation.x, (int)taskLocation.y);

			// sort nodes by nearest
			sort(allWorkers.begin(), allWorkers.end(),
				[&taskLocation](Node*  a, Node* b) -> bool
			{
				return a->GetTransform().localPos.distance(taskLocation) < b->GetTransform().localPos.distance(taskLocation);
			});

			if (task->GetType() == GameTaskType::ATTRACT) {
				float absCardinality = gameModel->CalcAttractorAbsCardinality(faction, task->GetTaskNode()->GetId());
				int neededDistance = absCardinality * 4;

				vector<GameMapTile*> nearestNodes;
				mapNode->FindWalkableNeighbors(neededDistance, nearestNodes);

				if (!nearestNodes.empty()) {
					vector<Node*> freeWorkers;
					// calculate number of free workers
					for (auto& worker : allWorkers) {
						if (assignedTasks.find(worker->GetId()) == assignedTasks.end()) {
							// if this worker is close to another attractor, skip him
							bool skip = false;
							vector<Node*> attractors;
							gameModel->GetAttractorsByFaction(faction, attractors);

							for (auto& attr : attractors) {
								if (attr->GetId() != task->GetTaskNode()->GetId()) {
									ofVec2f attrLoc = (attr->GetTransform().localPos) + 0.5f;
									if (attrLoc.distance(worker->GetTransform().localPos) < 5) {
										skip = true;
										break;
									}
								}
							}
							if (!skip) {
								freeWorkers.push_back(worker);
							}
						}
					}

					int workersToAssign = allWorkers.size()*absCardinality - task->GetReservedNodes().size();
					
					if (workersToAssign != 0) {
						workersToAssign = max((int)(workersToAssign / 1.8f), 1); // assign continuously :

						int workersAvailableToAsign = min(workersToAssign, (int)freeWorkers.size());

						for (int i = 0; i < workersAvailableToAsign; i++) {
							// prevent from other task to assign
							assignedTasks[freeWorkers[i]->GetId()]++;

							task->GetReservedNodes().push_back(freeWorkers[i]);
							task->SetIsReserved(true);
						}
					}
				}
			}
			else {
				bool workerFound = false;
				for (auto& worker : allWorkers) {

					// each player's worker can have only up to 4 tasks reserved; 
					if (assignedTasks.find(worker->GetId()) == assignedTasks.end()) {

						int alreadyAssigned = assignedTasks[worker->GetId()];
						
						if (alreadyAssigned <= 4 || (!playerModel->IsMultiplayer() && playerModel->GetFaction() != faction))
						{
							// try to find a path
							float manhattanDistance = abs(worker->GetTransform().localPos.x - taskLocation.x) + abs(worker->GetTransform().localPos.y - taskLocation.y);

							// position the worker stays
							auto nodeLocation = worker->GetTransform().localPos;

							if (task->GetType() == GameTaskType::BRIDGE_BUILD || task->GetType() == GameTaskType::BRIDGE_DESTROY) {
								if ((task->GetType() == GameTaskType::BRIDGE_BUILD && mapNode->GetMapTileType() == MapTileType::GROUND) ||
									task->GetType() == GameTaskType::BRIDGE_DESTROY && mapNode->GetMapTileType() == MapTileType::WATER) {
									gameModel->RemoveGameTask(task);
									break;
								}

								// find nearest node the worker will stay during the building (worker cannot go to the water)
								auto nodeToBuildfrom = mapNode->FindWalkableNeighbor(Vec2i(taskLocation.x, taskLocation.y));

								if (nodeToBuildfrom != nullptr) {
									vector<Vec2i> path;
									map->FindPath(Vec2i(nodeLocation.x, nodeLocation.y), nodeToBuildfrom->GetPosition(), true, path, 2 * manhattanDistance);

									if (!path.empty()) {
										// assign
										task->SetIsReserved(true);
										task->GetReservedNodes().push_back(worker);
										task->SetReservedTime(absolute);
										workerFound = true;
										break;
									}
								}
							}
						}
					}
				}


				if (!workerFound) {
					// no worker assigned -> try to find the complete path
					for (auto& worker : allWorkers) {

						// each worker can have only up to 2 tasks reserved
						if (assignedTasks.find(worker->GetId()) == assignedTasks.end() || assignedTasks[worker->GetId()] <= 2) {

							auto nodeLocation = worker->GetTransform().localPos;
							auto nodeToBuildfrom = mapNode->FindWalkableNeighbor(Vec2i(nodeLocation.x, nodeLocation.y));

							if (nodeToBuildfrom != nullptr) {
								vector<Vec2i> path;
								float manhattanDistance = abs(worker->GetTransform().localPos.x - taskLocation.x) 
									+ abs(worker->GetTransform().localPos.y - taskLocation.y);

								map->FindPath(Vec2i(nodeLocation.x, nodeLocation.y), nodeToBuildfrom->GetPosition(), true, path, 8*manhattanDistance);

								if (!path.empty()) {
									// assign
									task->SetIsReserved(true);
									task->GetReservedNodes().push_back(worker);
									task->SetReservedTime(absolute);
									workerFound = true;
									break;
								}
								else {
									// no path could be found -> set delay property
									task->SetIsDelayed(true);
									break;
								}
							}
						}
					}
				}


			}
		}
	}

	COGMEASURE_END("HYDROQ_SCHEDULER");
}