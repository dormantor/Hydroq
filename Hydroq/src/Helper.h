#pragma once

#include <string>


using namespace std;

enum class MapNodeType;

class Helper {
public:
	static MapNodeType GetMapNodeTypeByName(int& index, string name);

	static string GetMapNameByNodeType(MapNodeType type);

};