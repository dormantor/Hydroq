#include "GameMap.h"
#include "MapLoader.h"


// ===================================== GameMapNode ========================================


GameMapTile* GameMapTile::FindWalkableNeighbor(Vec2i preferredDirection) {
	if (preferredDirection.x == pos.x && preferredDirection.y <= pos.y) {
		if (top != nullptr && top->IsWalkable()) return top;
		if (left != nullptr && left->IsWalkable()) return left;
		if (right != nullptr && right->IsWalkable()) return right;
		if (bottom != nullptr && bottom->IsWalkable()) return bottom;
	}
	else if (preferredDirection.x == pos.x && preferredDirection.y > pos.y) {
		if (bottom != nullptr && bottom->IsWalkable()) return bottom;
		if (left != nullptr && left->IsWalkable()) return left;
		if (right != nullptr && right->IsWalkable()) return right;
		if (top != nullptr && top->IsWalkable()) return top;
	}
	else if (preferredDirection.x > pos.x && preferredDirection.y == pos.y) {
		if (right != nullptr && right->IsWalkable()) return right;
		if (top != nullptr && top->IsWalkable()) return top;
		if (bottom != nullptr && bottom->IsWalkable()) return bottom;
		if (left != nullptr && left->IsWalkable()) return left;
	}
	else if (preferredDirection.x <= pos.x && preferredDirection.y == pos.y) {
		if (left != nullptr && left->IsWalkable()) return left;
		if (top != nullptr && top->IsWalkable()) return top;
		if (bottom != nullptr && bottom->IsWalkable()) return bottom;
		if (right != nullptr && right->IsWalkable()) return right;
	}
	else if (preferredDirection.x <= pos.x && preferredDirection.y <= pos.y) {
		if (left != nullptr && left->IsWalkable()) return left;
		if (right != nullptr && right->IsWalkable()) return right;
		if (top != nullptr && top->IsWalkable()) return top;
		if (bottom != nullptr && bottom->IsWalkable()) return bottom;
	}
	else if (preferredDirection.x <= pos.x && preferredDirection.y > pos.y) {
		if (left != nullptr && left->IsWalkable()) return left;
		if (right != nullptr && right->IsWalkable()) return right;
		if (bottom != nullptr && bottom->IsWalkable()) return bottom;
		if (top != nullptr && top->IsWalkable()) return top;
	}
	else if (preferredDirection.x > pos.x && preferredDirection.y <= pos.y) {
		if (right != nullptr && right->IsWalkable()) return right;
		if (left != nullptr && left->IsWalkable()) return left;
		if (top != nullptr && top->IsWalkable()) return top;
		if (bottom != nullptr && bottom->IsWalkable()) return bottom;
	}
	else if (preferredDirection.x > pos.x && preferredDirection.y > pos.y) {
		if (right != nullptr && right->IsWalkable()) return right;
		if (left != nullptr && left->IsWalkable()) return left;
		if (bottom != nullptr && bottom->IsWalkable()) return bottom;
		if (top != nullptr && top->IsWalkable()) return top;
	}

	return nullptr;
}

void GameMapTile::FindWalkableNeighbors(int distance, vector<GameMapTile*>& output) {

	if (this->IsWalkable()) output.push_back(this);

	if (distance > 0) {
		if (top != nullptr) top->FindWalkableNeighbors(distance - 1, output);
		if (topRight != nullptr) topRight->FindWalkableNeighbors(distance - 1, output);
		if (right != nullptr) right->FindWalkableNeighbors(distance - 1, output);
		if (bottomRight != nullptr) bottomRight->FindWalkableNeighbors(distance - 1, output);
		if (bottom != nullptr) bottom->FindWalkableNeighbors(distance - 1, output);
		if (left != nullptr) left->FindWalkableNeighbors(distance - 1, output);
		if (topLeft != nullptr) topLeft->FindWalkableNeighbors(distance - 1, output);
		if (bottomLeft != nullptr) bottomLeft->FindWalkableNeighbors(distance - 1, output);
	}
}

GameMapTile* GameMapTile::FindNeighborByType(MapTileType type, Vec2i preferredDirection) {
	if (preferredDirection.x <= pos.x && preferredDirection.y <= pos.y) {
		if (top != nullptr && top->mapTileType == type) return top;
		if (left != nullptr && left->mapTileType == type) return left;
		if (right != nullptr && right->mapTileType == type) return right;
		if (bottom != nullptr && bottom->mapTileType == type) return bottom;
	}
	else if (preferredDirection.x <= pos.x && preferredDirection.y > pos.y) {
		if (bottom != nullptr && bottom->mapTileType == type) return bottom;
		if (left != nullptr && left->mapTileType == type) return left;
		if (right != nullptr && right->mapTileType == type) return right;
		if (top != nullptr && top->mapTileType == type) return top;
	}
	else if (preferredDirection.x > pos.x && preferredDirection.y <= pos.y) {
		if (top != nullptr && top->mapTileType == type) return top;
		if (right != nullptr && right->mapTileType == type) return right;
		if (left != nullptr && left->mapTileType == type) return left;
		if (bottom != nullptr && bottom->mapTileType == type) return bottom;
	}
	else if (preferredDirection.x > pos.x && preferredDirection.y > pos.y) {
		if (bottom != nullptr && bottom->mapTileType == type) return bottom;
		if (right != nullptr && right->mapTileType == type) return right;
		if (left != nullptr && left->mapTileType == type) return left;
		if (top != nullptr && top->mapTileType == type) return top;
	}

	return nullptr;
}

void GameMapTile::GetNeighbors(vector<GameMapTile*>& output) {
	if (top != nullptr) output.push_back(top);
	if (topRight != nullptr) output.push_back(topRight);
	if (right != nullptr) output.push_back(right);
	if (bottomRight != nullptr) output.push_back(bottomRight);
	if (bottom != nullptr) output.push_back(bottom);
	if (bottomLeft != nullptr) output.push_back(bottomLeft);
	if (left != nullptr) output.push_back(left);
	if (topLeft != nullptr) output.push_back(topLeft);
}

void GameMapTile::GetNeighborsFourDirections(vector<GameMapTile*>& output) {
	if (top != nullptr) output.push_back(top);
	if (right != nullptr) output.push_back(right);
	if (bottom != nullptr) output.push_back(bottom);
	if (left != nullptr) output.push_back(left);
}


// ===================================== GameMap ========================================

void GameMap::LoadMap(Settings mapConfig, string selectedMap) {

	this->mapConfig = mapConfig;

	// load path to map from config
	string mapPath = mapConfig.GetSettingVal("maps_files", selectedMap);

	if (mapPath.empty()) throw ConfigErrorException(string_format("Path to map %s not found", selectedMap.c_str()));
	
	// load map from PNG image
	MapLoader mapLoad = MapLoader();
	TileMap tileMap;
	mapLoad.LoadFromPNGImage(mapPath, mapConfig.GetSetting("names"), tileMap);
	this->LoadMap(tileMap);
}

void GameMap::LoadMap(TileMap& tileMap) {
	this->width = tileMap.width;
	this->height = tileMap.height;
	gridNoBlock = GridGraph(width, height);
	gridWithBlocks = GridGraph(width, height);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			
			Tile br = tileMap.GetTile(i, j);
			GameMapTile* tile = new GameMapTile();

			tile->mapTileName = br.name;
			tile->mapTileTypeIndex = br.index;
			tile->mapTileType = Helper::GetMapTileTypeByName(br.name);
			tile->pos = Vec2i(i, j);
			tiles.push_back(tile);

			if (tile->mapTileType == MapTileType::RIG && tile->mapTileTypeIndex == 0) {
				rigs.push_back(tile);
			}

			// add obstruction into grid for A* search
			if (!tile->IsWalkable()) {
				gridNoBlock.AddBlock(i, j);
				gridWithBlocks.AddBlock(i, j);
			}
		}
	}

	// assign neighbors
	for (auto tile : tiles) {
		RefreshTile(tile);
	}
}

void GameMap::RefreshTile(GameMapTile* tile) {
	int i = tile->pos.x;
	int j = tile->pos.y;

	// refresh neighbors
	if (i > 0) tile->left = GetTile(i - 1, j); // left
	if (i > 0 && j > 0) tile->topLeft = GetTile(i - 1, j - 1);	// topleft
	if (i > 0 && j < (height - 1)) tile->bottomLeft = GetTile(i - 1, j + 1); // bottomleft
	if (j > 0) tile->top = GetTile(i, j - 1); // top
	if (j < (height - 1)) tile->bottom = GetTile(i, j + 1); // bottom
	if (i < (width - 1)) tile->right = GetTile(i + 1, j); // right
	if (i < (width - 1) && j < (height - 1)) tile->bottomRight = GetTile(i + 1, j + 1); // bottomright
	if (i < (width - 1) && j > 0) tile->topRight = GetTile(i + 1, j - 1); // topright

	// fix A* grid (the map may have changed)
	if (tile->mapTileType == MapTileType::GROUND || tile->mapTileType == MapTileType::RIG_PLATFORM) {
		gridNoBlock.RemoveBlock(i, j);
		gridWithBlocks.RemoveBlock(i, j);
	}
	else {
		gridNoBlock.AddBlock(i, j);
		gridWithBlocks.AddBlock(i, j);
	}

	// map with forbidden areas is separate grid
	if (tile->forbidden) {
		gridWithBlocks.AddBlock(i, j);
	}
}

void GameMap::FindPath(Vec2i start, Vec2i end, bool crossForbiddenArea, vector<Vec2i>& output, int maxIteration) {

	COGMEASURE_BEGIN("HYDROQ_PATHFINDING");

	AStarSearch srch;
	AStarSearchContext context;

	// prefer grid with forbidden areas
	bool found = srch.Search(gridWithBlocks, start, end, context, maxIteration);
	// try it again for grid with forbidden areas
	if (!found && crossForbiddenArea) {
		context = AStarSearchContext();
		found = srch.Search(gridNoBlock, start, end, context, maxIteration);
	}

	if (found) {
		srch.CalcPathFromSteps(start, end, context.steps, output);
	}

	COGMEASURE_END("HYDROQ_PATHFINDING");
}

int GameMap::CalcNearestReachablePosition(Vec2i start, Vec2i end, Vec2i& nearestBlock, int maxIteration) {

	AStarSearch srch;
	AStarSearchContext context;

	// prefer grid with forbidden areas
	bool found = srch.Search(gridNoBlock, start, end, context, maxIteration);
	nearestBlock = context.nearestBlock;
	return context.nearestDistance;
}