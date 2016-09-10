#include "TaskScheduler.h"


map<int, int> TaskScheduler::CalcAssignedTasks(vector<spt<GameTask>>& tasks) {
	map<int, int> output = map<int, int>();

	for (auto& task : tasks) {
		if (task->isReserved) {
			for (auto node : task->reserverNodes) {
				if (output.find(node->GetId()) == output.end()) {
					output[node->GetId()] = 0;
				}
				else {
					output[node->GetId()] += 1;
				}
			}
		}
	}

	return output;
}

void TaskScheduler::ScheduleTasks(uint64 absolute) {

	auto allTasks = gameModel->GetGameTaskCopy();
	auto allWorkers = gameModel->GetMovingObjectsByType(EntityType::WORKER, gameModel->GetFaction());
	auto map = gameModel->GetMap();

	auto assignedTasks = CalcAssignedTasks(allTasks);

	COGMEASURE_BEGIN("HYDROQ_SCHEDULER");

	for (auto& task : allTasks) {
		
		if (!task->isDelayed && !task->isEnded && !task->isProcessing && 
			(!task->isReserved || (task->isReserved && (absolute - task->reserverTime) > 10000))
			&& (task->type == GameTaskType::BRIDGE_BUILD || task->type == GameTaskType::BRIDGE_DESTROY || task->type == GameTaskType::ATTRACT)) {
			
			auto taskLocation = task->taskNode->GetTransform().localPos;
			
			// node at position the bridge will stay
			auto mapNode = map->GetNode((int)taskLocation.x, (int)taskLocation.y);
			
			// sort nodes by nearest
			sort(allWorkers.begin(), allWorkers.end(),
				[&taskLocation](Node*  a, Node* b) -> bool
			{
				return a->GetTransform().localPos.distance(taskLocation) < b->GetTransform().localPos.distance(taskLocation);
			});

			if (task->type == GameTaskType::ATTRACT) {
				float absCardinality = gameModel->CalcAttractorAbsCardinality(task->taskNode->GetId());
				int neededDistance = absCardinality * 4;

				vector<HydMapNode*> nearestNodes = vector<HydMapNode*>();
				mapNode->FindWalkableNeighbor(neededDistance, nearestNodes);

				if (!nearestNodes.empty()) {
					vector<Node*> freeWorkers = vector<Node*>();
					// calculate number of free workers
					for (auto& worker : allWorkers) {
						if (assignedTasks.find(worker->GetId()) == assignedTasks.end()) {
							// if this worker is close to another attractor, skip him
							bool skip = false;
							for (auto& attr : gameModel->GetAttractors()) {
								if (attr.second->GetId() != task->taskNode->GetId()) {
									ofVec2f pos = ofVec2f(attr.first.x + 0.5f, attr.first.y + 0.5f);
									if (pos.distance(worker->GetTransform().localPos) < 5) {
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

					int workersToAssign = allWorkers.size()*absCardinality - task->reserverNodes.size();
					workersToAssign = max((int)(workersToAssign / 1.8f),1); // assign continuously :

					int workersAvailableToAsign = min(workersToAssign, (int)freeWorkers.size());

					for (int i = 0; i < workersAvailableToAsign; i++) {
						// prevent from other task to assign
						assignedTasks[freeWorkers[i]->GetId()]++;

						task->reserverNodes.push_back(freeWorkers[i]);
						task->isReserved = true;
						//task->reserverTime = absolute; NOT SET !
					}
				}
			}
			else {
				bool workerFound = false;
				for (auto& worker : allWorkers) {

					// each worker can have only up to 2 tasks reserved
					if (assignedTasks.find(worker->GetId()) == assignedTasks.end() || assignedTasks[worker->GetId()] <= 2) {

						// try to find a path
						float manhattanDistance = abs(worker->GetTransform().localPos.x - taskLocation.x) + abs(worker->GetTransform().localPos.y - taskLocation.y);

						// position the worker stays
						auto nodeLocation = worker->GetTransform().localPos;

						if (task->type == GameTaskType::BRIDGE_BUILD || task->type == GameTaskType::BRIDGE_DESTROY) {
							// find nearest node the worker will stay during the building (worker cannot go to the water)
							auto nodeToBuildfrom = mapNode->FindWalkableNeighbor(Vec2i(nodeLocation.x, nodeLocation.y));

							if (nodeToBuildfrom != nullptr) {
								vector<Vec2i> path = map->FindPath(Vec2i(nodeLocation.x, nodeLocation.y), nodeToBuildfrom->pos, true, 2 * manhattanDistance);

								if (!path.empty()) {
									// assign
									task->isReserved = true;
									task->reserverNodes.push_back(worker);
									task->reserverTime = absolute;
									workerFound = true;
									break;
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
								vector<Vec2i> path = map->FindPath(Vec2i(nodeLocation.x, nodeLocation.y), Vec2i(taskLocation.x, taskLocation.y), true, 0);

								if (!path.empty()) {
									// assign
									task->isReserved = true;
									task->reserverNodes.push_back(worker);
									task->reserverTime = absolute;
									workerFound = true;
									break;
								}
								else {
									// no path could be found as well -> set delay property
									task->isDelayed = true;
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