#include "TaskScheduler.h"


void TaskScheduler::ScheduleTasks() {

	auto allTasks = gameModel->GetGameTaskCopy();
	auto allWorkers = gameModel->GetMovingObjectsByType(EntityType::WORKER, true);
	auto map = gameModel->GetMap();

	COGMEASURE_BEGIN("HYDROQ_SCHEDULER");

	for (auto& task : allTasks) {
		
		if (!task->isEnded && !task->isProcessing &&!task->isReserved && 
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
				// try to find a path
				float manhattanDistance = abs(worker->GetTransform().localPos.x - taskLocation.x) + abs(worker->GetTransform().localPos.y - taskLocation.y);

				// position the worker stays
				auto nodeLocation = worker->GetTransform().localPos;
				// find nearest node the worker will stay during the building (worker cannot go to the water)
				auto nodeToBuildfrom = mapNode->FindNeighborByType(MapNodeType::GROUND, Vec2i(nodeLocation.x, nodeLocation.y));
				if (nodeToBuildfrom != nullptr) {
					vector<Vec2i> path = map->FindPath(Vec2i(nodeLocation.x, nodeLocation.y), nodeToBuildfrom->pos, true, 2 * manhattanDistance);

					if (!path.empty()) {
						// assign
						task->isReserved = true;
						task->reserverNode = worker;
						workerFound = true;
						break;
					}
				}
			}

			if (!workerFound) {
				// no worker assigned -> try to find the complete path
				for (auto& worker : allWorkers) {
					auto nodeLocation = worker->GetTransform().localPos;
					auto nodeToBuildfrom = mapNode->FindNeighborByType(MapNodeType::GROUND, Vec2i(nodeLocation.x, nodeLocation.y));

					if (nodeToBuildfrom != nullptr) {
						vector<Vec2i> path = map->FindPath(Vec2i(nodeLocation.x, nodeLocation.y), Vec2i(taskLocation.x, taskLocation.y), true, 0);

						if (!path.empty()) {
							// assign
							task->isReserved = true;
							task->reserverNode = worker;
							workerFound = true;
							break;
						}
					}
				}
			}
		}
	}

	COGMEASURE_END("HYDROQ_SCHEDULER");
}