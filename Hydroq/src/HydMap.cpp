#include "HydMap.h"

// ===================================== HydMapNode ========================================


HydMapNode* HydMapNode::FindWalkableNeighbor(Vec2i preferredPosition) {

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

void HydMapNode::FindWalkableNeighbor(int distance, vector<HydMapNode*>& output) {

	if (this->IsWalkable()) output.push_back(this);

	if (distance > 0) {
		if (top != nullptr) top->FindWalkableNeighbor(distance - 1, output);
		if (topRight != nullptr) topRight->FindWalkableNeighbor(distance - 1, output);
		if (right != nullptr) right->FindWalkableNeighbor(distance - 1, output);
		if (bottomRight != nullptr) bottomRight->FindWalkableNeighbor(distance - 1, output);
		if (bottom != nullptr) bottom->FindWalkableNeighbor(distance - 1, output);
		if (left != nullptr) left->FindWalkableNeighbor(distance - 1, output);
		if (topLeft != nullptr) topLeft->FindWalkableNeighbor(distance - 1, output);
		if (bottomLeft != nullptr) bottomLeft->FindWalkableNeighbor(distance - 1, output);
	}
}

HydMapNode* HydMapNode::FindNeighborByType(MapNodeType type, Vec2i preferredPosition) {
	if (preferredPosition.x <= pos.x && preferredPosition.y <= pos.y) {
		if (top != nullptr && top->mapNodeType == type) return top;
		if (left != nullptr && left->mapNodeType == type) return left;
		if (right != nullptr && right->mapNodeType == type) return right;
		if (bottom != nullptr && bottom->mapNodeType == type) return bottom;
	}
	else if (preferredPosition.x <= pos.x && preferredPosition.y > pos.y) {
		if (bottom != nullptr && bottom->mapNodeType == type) return bottom;
		if (left != nullptr && left->mapNodeType == type) return left;
		if (right != nullptr && right->mapNodeType == type) return right;
		if (top != nullptr && top->mapNodeType == type) return top;
	}
	else if (preferredPosition.x > pos.x && preferredPosition.y <= pos.y) {
		if (top != nullptr && top->mapNodeType == type) return top;
		if (right != nullptr && right->mapNodeType == type) return right;
		if (left != nullptr && left->mapNodeType == type) return left;
		if (bottom != nullptr && bottom->mapNodeType == type) return bottom;
	}
	else if (preferredPosition.x > pos.x && preferredPosition.y > pos.y) {
		if (bottom != nullptr && bottom->mapNodeType == type) return bottom;
		if (right != nullptr && right->mapNodeType == type) return right;
		if (left != nullptr && left->mapNodeType == type) return left;
		if (top != nullptr && top->mapNodeType == type) return top;
	}

	return nullptr;
}

vector<HydMapNode*> HydMapNode::GetNeighbors() {
	vector<HydMapNode*> output;
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

vector<HydMapNode*> HydMapNode::GetNeighborsFourDirections() {
	vector<HydMapNode*> output;
	if (top != nullptr) output.push_back(top);
	if (right != nullptr) output.push_back(right);
	if (bottom != nullptr) output.push_back(bottom);
	if (left != nullptr) output.push_back(left);

	return output;
}


// ===================================== HydMap ========================================

void HydMap::LoadMap(Settings mapConfig, string selectedMap) {

	this->mapConfig = mapConfig;

	string mapPath = mapConfig.GetSettingVal("maps_files", selectedMap);

	if (mapPath.empty()) throw ConfigErrorException(string_format("Path to map %s not found", selectedMap.c_str()));
	// load map
	MapLoader mapLoad = MapLoader();
	TileMap tileMap;
	mapLoad.LoadFromPNGImage(mapPath, mapConfig.GetSetting("names"), tileMap);
	this->LoadMap(tileMap);
}

void HydMap::LoadMap(TileMap& brickMap) {
	this->width = brickMap.width;
	this->height = brickMap.height;
	gridNoBlock = GridGraph(width, height);
	gridWithBlocks = GridGraph(width, height);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			Tile br = brickMap.GetTile(i, j);

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
}

void HydMap::RefreshNode(HydMapNode* node) {
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

vector<Vec2i> HydMap::FindPath(Vec2i start, Vec2i end, bool crossForbiddenArea, int maxIteration ) {

	COGMEASURE_BEGIN("HYDROQ_PATHFINDING");

	AStarSearch srch;
	AStarSearchContext context;

	// prefer grid with forbidden areas
	bool found = srch.Search(gridWithBlocks, start, end, context, maxIteration);
	if (!found && crossForbiddenArea) {
		context = AStarSearchContext();
		found = srch.Search(gridNoBlock, start, end, context, maxIteration);
	}

	if (found) {
		vector<Vec2i> path; 
		srch.CalcPathFromSteps(start, end, context.steps, path);
		COGMEASURE_END("HYDROQ_PATHFINDING");
		return path;
	}
	else {
		COGMEASURE_END("HYDROQ_PATHFINDING");
		return vector<Vec2i>();
	}
}

int HydMap::CalcNearestReachablePosition(Vec2i start, Vec2i end, Vec2i& nearestBlock, int maxIteration) {

	AStarSearch srch;
	AStarSearchContext context;

	// prefer grid with forbidden areas
	bool found = srch.Search(gridNoBlock, start, end, context, maxIteration);
	nearestBlock = context.nearestBlock;
	return context.nearestDistance;
}