#pragma once

#include "ofxCogMain.h"
#include "BrickClickEvent.h"
#include "HydroqDef.h"
#include "Helper.h"

enum class MapNodeType {
	NONE, WATER, GROUND, RIG, 
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

public:
	void LoadMap(BrickMap* brickMap) {
		this->width = brickMap->width;
		this->height = brickMap->height;

		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				Brick br = brickMap->GetBrick(i, j);
			
				HydMapNode* node = new HydMapNode();
				int index = 0;
				
				node->mapNodeName = br.name;
				node->mapNodeTypeIndex = index;
				node->mapNodeType = Helper::GetMapNodeTypeByName(index, br.name);
				node->pos = Vec2i(i, j);
				nodes.push_back(node);
			}
		}

		// assign neighbors
		for (auto node : nodes) {
			RecalcNeighbors(node);
		}
	}

	void RecalcNeighbors(HydMapNode* node) {
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
	}

	HydMapNode* GetNode(int x, int y) {
		return nodes[y*width + x];
	}

	int GetWidth() {
		return width;
	}

	int GetHeight() {
		return height;
	}
};