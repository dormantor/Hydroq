#pragma once

#include "ofxCogMain.h"
#include <string>
#include "StringHash.h"

using namespace std;

enum class MapNodeType;

class Helper {
public:
	static MapNodeType GetMapNodeTypeByName(int& index, string name);

	static string GetMapNameByNodeType(MapNodeType type);

	static void SetPanelItem(Node* owner, Node* listNode, int index, StringHash selectionGroup, StringHash idAttr, string idValue, bool select = false);

};