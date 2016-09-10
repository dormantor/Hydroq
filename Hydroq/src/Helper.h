#pragma once

#include "ofxCogMain.h"
#include <string>
#include "StrId.h"

using namespace std;

enum class MapNodeType;

class Helper {
public:
	static MapNodeType GetMapNodeTypeByName(string name);

	static string GetMapNameByNodeType(MapNodeType type);

	static void SetPanelItem(Node* owner, Node* listNode, int index, StrId selectionGroup, StrId idAttr, string idValue, bool select = false);

};