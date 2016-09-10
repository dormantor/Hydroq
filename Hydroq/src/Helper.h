#pragma once

#include <string>
#include "StrId.h"

using namespace Cog;
namespace Cog {
	class Node;
}

enum class MapTileType;

/**
* Static class with helping methods
*/
class Helper {
public:
	/**
	* Gets type of maptile by its name
	*/
	static MapTileType GetMapTileTypeByName(string name);

	/**
	* Gets name of maptile by its type
	*/
	static string GetMapNameByTileType(MapTileType type);

	/**
	* Sets item of a panel - simulates listview
	*/
	static void SetPanelItem(Node* owner, Node* listNode, int index, 
		StrId selectionGroup, StrId idAttr, string idValue, bool select = false);

};