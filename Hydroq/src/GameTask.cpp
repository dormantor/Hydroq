#include "GameTask.h"
#include "Node.h"

bool GameTask::IsNodeReserved(int nodeId) {
	for (auto nd : reserverNodes) {
		if (nd->GetId() == nodeId) return true;
	}
	return false;
}

void GameTask::RemoveReserverNode(int nodeId) {
	for (auto it = reserverNodes.begin(); it != reserverNodes.end(); ++it) {
		if ((*it)->GetId() == nodeId) {
			reserverNodes.erase(it);
			break;
		}
	}
}