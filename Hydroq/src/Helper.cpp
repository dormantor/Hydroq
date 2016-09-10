#include "Helper.h"
#include "HydMap.h"

 MapNodeType Helper::GetMapNodeTypeByName(int& index, string name) {
	index = 0;

	if (name.compare("water") == 0) return MapNodeType::WATER;
	else if (name.compare("ground") == 0) return MapNodeType::GROUND;
	else if (name.compare("rig_topleft") == 0) {
		index = 0;
		return MapNodeType::RIG;
	}
	else if (name.compare("rig_topright") == 0) {
		index = 1;
		return MapNodeType::RIG;
	}
	else if (name.compare("rig_botleft") == 0) {
		index = 2;
		return MapNodeType::RIG;
	}
	else if (name.compare("rig_botright") == 0) {
		index = 3;
		return MapNodeType::RIG;
	}

	return MapNodeType::NONE;
}