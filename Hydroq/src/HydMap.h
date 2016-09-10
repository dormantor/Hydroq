#pragma once

#include "ofxCogMain.h"
#include "TileClickEvent.h"
#include "HydroqDef.h"
#include "Helper.h"
#include "AStarSearch.h"


enum class Faction {
	NONE = 1, 
	RED = 2, 
	BLUE = 3
};


enum class MapNodeType {
	NONE, WATER, GROUND, RIG, RIG_PLATFORM
};

class HydMapNode {
public:
	HydMapNode* top = nullptr;
	HydMapNode* topRight = nullptr;
	HydMapNode* right = nullptr;
	HydMapNode* bottomRight = nullptr;
	HydMapNode* bottom = nullptr;
	HydMapNode* bottomLeft = nullptr;
	HydMapNode* left = nullptr;
	HydMapNode* topLeft = nullptr;

	MapNodeType mapNodeType;
	int mapNodeTypeIndex; // for rigs and partial objects
	string mapNodeName;
	bool occupied = false; // true, if is occupied by a building
	bool forbidden = false; // true, if it is forbidden
	Vec2i pos;

	void ChangeMapNodeType(MapNodeType newType) {
		this->mapNodeType = newType;
		this->mapNodeName = Helper::GetMapNameByNodeType(newType);
	}

	bool IsWalkable() {
		return this->mapNodeType == MapNodeType::GROUND || this->mapNodeType == MapNodeType::RIG_PLATFORM;
	}


	HydMapNode* FindWalkableNeighbor(Vec2i preferredPosition);

	void FindWalkableNeighbor(int distance, vector<HydMapNode*>& output);

	HydMapNode* FindNeighborByType(MapNodeType type, Vec2i preferredPosition);

	vector<HydMapNode*> GetNeighbors();

	vector<HydMapNode*> GetNeighborsFourDirections();
};


class HydMap {
private:
	int width;
	int height;
	vector<HydMapNode*> nodes;
	// grid for searching algorithms
	GridGraph gridNoBlock; // grid without forbidden areas
	GridGraph gridWithBlocks; // grid with forbidden areas
	vector<HydMapNode*> rigs;
	Settings mapConfig;

public:

	void LoadMap(Settings mapConfig, string selectedMap);

	void LoadMap(TileMap& brickMap);

	void RefreshNode(Vec2i position) {
		RefreshNode(GetNode(position.x, position.y));
	}

	void RefreshNode(HydMapNode* node);

	vector<Vec2i> FindPath(Vec2i start, Vec2i end, bool crossForbiddenArea, int maxIteration = 0);

	int CalcNearestReachablePosition(Vec2i start, Vec2i end, Vec2i& nearestBlock, int maxIteration);

	HydMapNode* GetNode(int x, int y) {
		return nodes[y*width + x];
	}

	HydMapNode* GetNode(Vec2i pos) {
		return nodes[pos.y*width + pos.x];
	}

	vector<Vec2i> GetRigsPositions() {
		vector<Vec2i> output;
		
		for (auto node : this->rigs) {
			output.push_back(node->pos);
		}
		return output;
	}

	int GetWidth() {
		return width;
	}

	int GetHeight() {
		return height;
	}

	Settings& GetMapConfig() {
		return this->mapConfig;
	}
};