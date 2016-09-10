#pragma once

#include "HydroqDef.h"
#include "Helper.h"
#include "AStarSearch.h"
#include "Settings.h"
#include "Vec2i.h"
#include "Tile.h"

using namespace Cog;

enum class MapNodeType {
	NONE, WATER, GROUND, RIG, RIG_PLATFORM
};

class GameMapNode {
public:
	GameMapNode* top = nullptr;
	GameMapNode* topRight = nullptr;
	GameMapNode* right = nullptr;
	GameMapNode* bottomRight = nullptr;
	GameMapNode* bottom = nullptr;
	GameMapNode* bottomLeft = nullptr;
	GameMapNode* left = nullptr;
	GameMapNode* topLeft = nullptr;

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


	GameMapNode* FindWalkableNeighbor(Vec2i preferredPosition);

	void FindWalkableNeighbor(int distance, vector<GameMapNode*>& output);

	GameMapNode* FindNeighborByType(MapNodeType type, Vec2i preferredPosition);

	vector<GameMapNode*> GetNeighbors();

	vector<GameMapNode*> GetNeighborsFourDirections();
};


class GameMap {
private:
	int width;
	int height;
	vector<GameMapNode*> nodes;
	// grid for searching algorithms
	GridGraph gridNoBlock; // grid without forbidden areas
	GridGraph gridWithBlocks; // grid with forbidden areas
	vector<GameMapNode*> rigs;
	Settings mapConfig;

public:

	void LoadMap(Settings mapConfig, string selectedMap);

	void LoadMap(TileMap& brickMap);

	void RefreshNode(Vec2i position) {
		RefreshNode(GetNode(position.x, position.y));
	}

	void RefreshNode(GameMapNode* node);

	vector<Vec2i> FindPath(Vec2i start, Vec2i end, bool crossForbiddenArea, int maxIteration = 0);

	int CalcNearestReachablePosition(Vec2i start, Vec2i end, Vec2i& nearestBlock, int maxIteration);

	GameMapNode* GetNode(int x, int y) {
		return nodes[y*width + x];
	}

	GameMapNode* GetNode(Vec2i pos) {
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