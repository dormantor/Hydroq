#include "GameTask.h"
#include "Node.h"

bool GameTask::IsNodeReserved(int nodeId) {
	for (auto nd : reservedNodes) {
		if (nd->GetId() == nodeId) return true;
	}
	return false;
}

void GameTask::RemoveReserverNode(int nodeId) {
	for (auto it = reservedNodes.begin(); it != reservedNodes.end(); ++it) {
		if ((*it)->GetId() == nodeId) {
			reservedNodes.erase(it);
			break;
		}
	}
}