#pragma once

#include "HydroqDef.h"
#include "Helper.h"
#include "AStarSearch.h"
#include "Settings.h"
#include "Vec2i.h"
#include "Tile.h"

using namespace Cog;

/**
* Type of map tile
*/
enum class MapTileType {
	NONE,		/** undefined */
	WATER,		/** water */
	GROUND,		/** bridge/ground*/
	RIG,		/** drilling rig*/
	RIG_PLATFORM/** platform (the area closest to rig)*/
};

/**
* Tile of game map
*/
class GameMapTile {
private:
	// type of tile
	MapTileType mapTileType;
	// index of tile (e.g. drilling rig has 4 parts)
	int mapTileTypeIndex;
	// name of tile
	string mapTileName;
	// indicator whether the tile is occupied by a building
	bool occupied = false;
	// indicator whether the tile is forbidden to cross
	bool forbidden = false;
	// position of tile
	Vec2i pos;
public:

	// neighbors
	GameMapTile* top = nullptr;
	GameMapTile* topRight = nullptr;
	GameMapTile* right = nullptr;
	GameMapTile* bottomRight = nullptr;
	GameMapTile* bottom = nullptr;
	GameMapTile* bottomLeft = nullptr;
	GameMapTile* left = nullptr;
	GameMapTile* topLeft = nullptr;

	/**
	* Gets type of the tile
	*/
	MapTileType GetMapTileType() const {
		return mapTileType;
	}

	/**
	* Sets type of the tile
	*/
	void SetMapTileType(MapTileType newType) {
		this->mapTileType = newType;
		this->mapTileName = Helper::GetMapNameByTileType(newType);
	}

	/**
	* Gets index of the map tile 
	* (e.g. rigs consist of 4 parts) 
	*/
	int GetMapTileTypeIndex() const {
		return mapTileTypeIndex;
	}

	/**
	* Sets index of the map tile
	*/
	void SetMapTileTypeIndex(int index) {
		this->mapTileTypeIndex = index;
	}

	/**
	* Gets name of the map tile
	*/
	string GetMapTileName() const {
		return mapTileName;
	}

	/**
	* Sets name of the tile
	*/
	void SetMapTileName(string name) {
		this->mapTileName = name;
	}

	/**
	* Gets indicator whether the tile is occupied
	*/
	bool IsOccupied() const {
		return occupied;
	}

	/**
	* Sets indicator whether the tile is occupied
	*/
	void SetIsOccupied(bool occupied) {
		this->occupied = occupied;
	}

	/**
	* Gets indicator whether the tile is forbidden
	*/
	bool IsForbidden() const {
		return forbidden;
	}

	/**
	* Sets indicator whether the tile is forbidden
	*/
	void SetIsForbidden(bool forbidden) {
		this->forbidden = forbidden;
	}

	/**
	* Gets indicator whether the tile is walkable
	*/
	bool IsWalkable() const {
		return this->mapTileType == MapTileType::GROUND || this->mapTileType == MapTileType::RIG_PLATFORM;
	}

	/**
	* Gets position of the tile
	*/
	Vec2i GetPosition() const {
		return pos;
	}

	/**
	* Sets position of the tile
	*/
	void SetPosition(Vec2i pos) {
		this->pos = pos;
	}

	/**
	* Finds neighbor that can be crossed
	* @param preferredDirection direction that should be explored first 
	*/
	GameMapTile* FindWalkableNeighbor(Vec2i preferredDirection);

	/**
	* Finds list of all walkable neighbors
	* @param distance radius to explore (recursion is used here)
	* @param output output collection
	*/
	void FindWalkableNeighbors(int distance, vector<GameMapTile*>& output);

	/**
	* Finds first neighbor by type
	* @param type type of neighbor to find
	* @param preferredDirection direction that should be explored first
	*/
	GameMapTile* FindNeighborByType(MapTileType type, Vec2i preferredDirection);

	/**
	* Gets list of all neighbors in 8 directions
	*/
	void GetNeighbors(vector<GameMapTile*>& output);

	/**
	* Gets list of neighbors in 4 directions (top, right, bottom, left)
	*/
	void GetNeighborsFourDirections(vector<GameMapTile*>& output);

	friend class GameMap;
};

/**
* Abstract representation of the game map,
* contains only information for game model, not the view
*/
class GameMap {
private:
	// width of the map (in number of tiles)
	int width;
	// height of the map (in number of tiles)
	int height;
	// collection of all tiles
	vector<GameMapTile*> tiles;
	// grid without forbidden areas
	GridGraph gridNoBlock; 
	// grid with forbidden areas
	GridGraph gridWithBlocks; 
	// collection of drilling rigs
	vector<GameMapTile*> rigs;
	// map configuration
	Settings mapConfig;

public:

	/**
	* Loads map from configuration
	* @param mapConfig configuration of maps
	* @param selectedMap selected map to load
	*/
	void LoadMap(Settings mapConfig, string selectedMap);

	/**
	* Loads game map from tile map
	*/
	void LoadMap(TileMap& tileMap);

	/**
	* Refreshes tile at selected position
	*/
	void RefreshTile(Vec2i position) {
		RefreshTile(GetTile(position.x, position.y));
	}

	/**
	* Refreshes selected tile
	*/
	void RefreshTile(GameMapTile* tile);

	/**
	* Finds path from start to end, using A-star search algorithm
	* @param start start position
	* @param end final position
	* @param crossForbiddenArea if true, forbidden area may be crossed
	* @param output output collection of steps
	* @param maxIteration maximal number of iterations of the A* algorithm (0 for infinite)
	*/
	void FindPath(Vec2i start, Vec2i end, bool crossForbiddenArea, vector<Vec2i>& output, int maxIteration = 0);

	/**
	* Calculates nearest reachable position between starting and final position
	* @param start start position
	* @param end final position
	* @param nearestBlock output position of the nearest tile (if the path has been found, the value will be equal to final position)
	* @param maxIteration maximal number of iterations of the A* algorithm (0 for infinite)
	*/
	int CalcNearestReachablePosition(Vec2i start, Vec2i end, Vec2i& nearestBlock, int maxIteration);

	/**
	* Gets tile at selected position
	*/
	GameMapTile* GetTile(int x, int y) const {
		return tiles[y*width + x];
	}

	/**
	* Gets tile at selected position
	*/
	GameMapTile* GetTile(Vec2i pos) const {
		return tiles[pos.y*width + pos.x];
	}

	/**
	* Returns collection of positions of all rigs
	*/
	vector<Vec2i> GetRigsPositions() const {
		vector<Vec2i> output;
		
		for (auto node : this->rigs) {
			output.push_back(node->pos);
		}
		return output;
	}

	/**
	* Gets width of the map (in number of tiles)
	*/
	int GetWidth() const {
		return width;
	}

	/**
	* Gets height of the map (in number of tiles)
	*/
	int GetHeight() const {
		return height;
	}

	/**
	* Gets map configuration
	*/
	Settings& GetMapConfig() {
		return this->mapConfig;
	}
};