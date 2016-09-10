#include "TaskScheduler.h"


map<int, int> TaskScheduler::CalcAssignedTasks(vector<spt<GameTask>>& tasks) {
	map<int, int> output = map<int, int>();

	for (auto& task : tasks) {
		if (task->isReserved) {
			if (output.find(task->reserverNode->GetId()) == output.end()) {
				output[task->reserverNode->GetId()] = 0;
			}
			else {
				output[task->reserverNode->GetId()] += 1;
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
		
		if (!task->isEnded && !task->isProcessing && 
			(!task->isReserved || (task->isReserved && (absolute - task->reserverTime) > 10000))
			&& 
			(task->type == GameTaskType::BRIDGE_BUILD || task->type == GameTaskType::BRIDGE_DESTROY)) {
			
			auto taskLocation = task->taskNode->GetTransform().localPos;
			
			// node at position the bridge will stay
			auto mapNode = map->GetNode((int)taskLocation.x, (int)taskLocation.y);
			
			// sort nodes by nearest
			sort(allWorkers.begin(), allWorkers.end(),
				[&taskLocation](Node*  a, Node* b) -> bool
			{
				return a->GetTransform().localPos.distance(taskLocation) < b->GetTransform().localPos.distance(taskLocation);
			});

			bool workerFound = false;

			for (auto& worker : allWorkers) {

				// each worker can have only up to 2 tasks reserved
				if (assignedTasks.find(worker->GetId()) == assignedTasks.end() || assignedTasks[worker->GetId()] <= 2) {

					// try to find a path
					float manhattanDistance = abs(worker->GetTransform().localPos.x - taskLocation.x) + abs(worker->GetTransform().localPos.y - taskLocation.y);

					// position the worker stays
					auto nodeLocation = worker->GetTransform().localPos;
					// find nearest node the worker will stay during the building (worker cannot go to the water)
					auto nodeToBuildfrom = mapNode->FindWalkableNeighbor(Vec2i(nodeLocation.x, nodeLocation.y));

					if (nodeToBuildfrom != nullptr) {
						vector<Vec2i> path = map->FindPath(Vec2i(nodeLocation.x, nodeLocation.y), nodeToBuildfrom->pos, true, 2 * manhattanDistance);

						if (!path.empty()) {
							// assign
							task->isReserved = true;
							task->reserverNode = worker;
							task->reserverTime = absolute;
							workerFound = true;
							break;
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
								task->reserverNode = worker;
								task->reserverTime = absolute;
								workerFound = true;
								break;
							}
						}
					}
				}
			}
		}
	}

	COGMEASURE_END("HYDROQ_SCHEDULER");
}