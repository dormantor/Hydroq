#pragma once

#include <map>
#include "Vec2i.h"
#include "Definitions.h"

class FactionHolding {
public:
	int blueNumber = 0;
	int redNumber = 0;
};

/**
* Drilling rig entity
*/
class Rig {
public:
	// position of rig
	Vec2i position;
	// faction and number workers staying at each platform near the rig
	map<Vec2i, FactionHolding> factionHoldings;
	// link to game node
	Node* gameNode = nullptr;
	// collection of spawned worker
	vector<Node*> spawnedWorkers;
	// coordinates of all platforms that belong to the rig
	vector<Vec2i> platforms; 
};