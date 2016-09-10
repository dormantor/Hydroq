#pragma once

#include "ofxCogMain.h"
#include "BrickClickEvent.h"
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


	HydMapNode* FindWalkableNeighbor(Vec2i preferredPosition) {

		if (preferredPosition.x <= pos.x && preferredPosition.y <= pos.y) {
			if (top != nullptr && top->IsWalkable()) return top;
			if (left != nullptr && left->IsWalkable()) return left;
			if (right != nullptr && right->IsWalkable()) return right;
			if (bottom != nullptr && bottom->IsWalkable()) return bottom;
		}
		else if (preferredPosition.x <= pos.x && preferredPosition.y > pos.y) {
			if (bottom != nullptr && bottom->IsWalkable()) return bottom;
			if (left != nullptr && left->IsWalkable()) return left;
			if (right != nullptr && right->IsWalkable()) return right;
			if (top != nullptr && top->IsWalkable()) return top;
		}
		else if (preferredPosition.x > pos.x && preferredPosition.y <= pos.y) {
			if (top != nullptr && top->IsWalkable()) return top;
			if (right != nullptr && right->IsWalkable()) return right;
			if (left != nullptr && left->IsWalkable()) return left;
			if (bottom != nullptr && bottom->IsWalkable()) return bottom;
		}
		else if (preferredPosition.x > pos.x && preferredPosition.y > pos.y) {
			if (bottom != nullptr && bottom->IsWalkable()) return bottom;
			if (right != nullptr && right->IsWalkable()) return right;
			if (left != nullptr && left->IsWalkable()) return left;
			if (top != nullptr && top->IsWalkable()) return top;
		}

		return nullptr;
	}

	HydMapNode* FindNeighborByType(MapNodeType type, Vec2i preferredPosition) {	
		if (preferredPosition.x <= pos.x && preferredPosition.y <= pos.y) {
			if (top != nullptr && top->mapNodeType == type) return top;
			if (left != nullptr && left->mapNodeType == type) return left;
			if (right != nullptr && right->mapNodeType == type) return right;
			if (bottom != nullptr && bottom->mapNodeType == type) return bottom;
		}else if (preferredPosition.x <= pos.x && preferredPosition.y > pos.y) {
			if (bottom != nullptr && bottom->mapNodeType == type) return bottom;
			if (left != nullptr && left->mapNodeType == type) return left;
			if (right != nullptr && right->mapNodeType == type) return right;
			if (top != nullptr && top->mapNodeType == type) return top;
		}else if (preferredPosition.x > pos.x && preferredPosition.y <= pos.y) {
			if (top != nullptr && top->mapNodeType == type) return top;
			if (right != nullptr && right->mapNodeType == type) return right;
			if (left != nullptr && left->mapNodeType == type) return left;
			if (bottom != nullptr && bottom->mapNodeType == type) return bottom;
		}else if (preferredPosition.x > pos.x && preferredPosition.y > pos.y) {
			if (bottom != nullptr && bottom->mapNodeType == type) return bottom;
			if (right != nullptr && right->mapNodeType == type) return right;
			if (left != nullptr && left->mapNodeType == type) return left;
			if (top != nullptr && top->mapNodeType == type) return top;
		}

		return nullptr;
	}

	vector<HydMapNode*> GetNeighbors() {
		vector<HydMapNode*> output = vector<HydMapNode*>();
		if (top != nullptr) output.push_back(top);
		if (topRight != nullptr) output.push_back(topRight);
		if (right != nullptr) output.push_back(right);
		if (bottomRight != nullptr) output.push_back(bottomRight);
		if (bottom != nullptr) output.push_back(bottom);
		if (bottomLeft != nullptr) output.push_back(bottomLeft);
		if (left != nullptr) output.push_back(left);
		if (topLeft != nullptr) output.push_back(topLeft);

		return output;
	}
};


class HydMap {
private:
	int width;
	int height;
	vector<HydMapNode*> nodes;
	// grid for searching algorithms
	Grid gridNoBlock; // grid without forbidden areas
	Grid gridWithBlocks; // grid with forbidden areas
	vector<HydMapNode*> rigs;
	Settings mapConfig;

public:

	void LoadMap(Settings mapConfig, string selectedMap) {
		
		this->mapConfig = mapConfig;

		string mapPath = mapConfig.GetSettingVal("maps_files", selectedMap);

		if (mapPath.empty()) throw ConfigErrorException(string_format("Path to map %s not found", selectedMap.c_str()));
		// load map
		MapLoader mapLoad = MapLoader();
		this->LoadMap(mapLoad.LoadFromPNGImage(mapPath, mapConfig.GetSetting("names")));
	}

	void LoadMap(BrickMap* brickMap) {
		this->width = brickMap->width;
		this->height = brickMap->height;
		gridNoBlock = Grid(width, height);
		gridWithBlocks = Grid(width, height);

		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				Brick br = brickMap->GetBrick(i, j);
			
				HydMapNode* node = new HydMapNode();
				
				node->mapNodeName = br.name;
				node->mapNodeTypeIndex = br.index;
				node->mapNodeType = Helper::GetMapNodeTypeByName(br.name);
				node->pos = Vec2i(i, j);
				nodes.push_back(node);

				if (node->mapNodeType == MapNodeType::RIG && node->mapNodeTypeIndex == 0) {
					rigs.push_back(node);
				}

				// add obstruction into grid for A* search
				if (!node->IsWalkable()) {
					gridNoBlock.AddBlock(i, j);
					gridWithBlocks.AddBlock(i, j);
				}
			}
		}

		// assign neighbors
		for (auto node : nodes) {
			RefreshNode(node);
		}

		delete brickMap;
	}

	void RefreshNode(Vec2i position) {
		RefreshNode(GetNode(position.x, position.y));
	}

	void RefreshNode(HydMapNode* node) {
		int i = node->pos.x;
		int j = node->pos.y;

		if (i > 0) node->left = GetNode(i - 1, j); // left
		if (i > 0 && j > 0) node->topLeft = GetNode(i - 1, j - 1);	// topleft
		if (i > 0 && j < (height - 1)) node->bottomLeft = GetNode(i - 1, j + 1); // bottomleft
		if (j > 0) node->top = GetNode(i, j - 1); // top
		if (j < (height - 1)) node->bottom = GetNode(i, j + 1); // bottom
		if (i < (width - 1)) node->right = GetNode(i + 1, j); // right
		if (i < (width - 1) && j < (height - 1)) node->bottomRight = GetNode(i + 1, j + 1); // bottomright
		if (i < (width - 1) && j > 0) node->topRight = GetNode(i + 1, j - 1); // topright

		if (node->mapNodeType == MapNodeType::GROUND || node->mapNodeType == MapNodeType::RIG_PLATFORM) {
			gridNoBlock.RemoveBlock(i, j);
			gridWithBlocks.RemoveBlock(i, j);
		}
		else {
			gridNoBlock.AddBlock(i, j);
			gridWithBlocks.AddBlock(i, j);
		}

		if (node->forbidden) {
			gridWithBlocks.AddBlock(i, j);
		}
	}

	vector<Vec2i> FindPath(Vec2i start, Vec2i end, bool crossForbiddenArea, int maxIteration = 0) {

		COGMEASURE_BEGIN("HYDROQ_PATHFINDING");

		AStarSearch srch;
		unordered_map<Vec2i, Vec2i> came_from;
		unordered_map<Vec2i, int> cost_so_far;
		// prefer grid with forbidden areas
		bool found = srch.Search(gridWithBlocks, start, end, came_from, cost_so_far, maxIteration);
		if (!found && crossForbiddenArea) {
			came_from.clear();
			cost_so_far.clear();
			found = srch.Search(gridNoBlock, start, end, came_from, cost_so_far, maxIteration);
		}

		if (found) {
			vector<Vec2i> path = srch.CalcPathFromJumps(start, end, came_from);
			COGMEASURE_END("HYDROQ_PATHFINDING");
			return path;
		}
		else {
			COGMEASURE_END("HYDROQ_PATHFINDING");
			return vector<Vec2i>();
		}
	}

	HydMapNode* GetNode(int x, int y) {
		return nodes[y*width + x];
	}

	HydMapNode* GetNode(Vec2i pos) {
		return nodes[pos.y*width + pos.x];
	}

	vector<Vec2i> GetRigsPositions() {
		vector<Vec2i> output = vector<Vec2i>();
		
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