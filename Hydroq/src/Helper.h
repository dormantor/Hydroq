#pragma once

#include <string>
#include "StrId.h"

using namespace Cog;
namespace Cog {
	class Node;
}

enum class MapNodeType;

/**
* Static class with helping methods
*/
class Helper {
public:
	/**
	* Gets type of mapnode by its name
	*/
	static MapNodeType GetMapNodeTypeByName(string name);

	/**
	* Gets name of mapnode by its type
	*/
	static string GetMapNameByNodeType(MapNodeType type);

	/**
	* Sets item of a panel - simulates listview
	*/
	static void SetPanelItem(Node* owner, Node* listNode, int index, 
		StrId selectionGroup, StrId idAttr, string idValue, bool select = false);

};