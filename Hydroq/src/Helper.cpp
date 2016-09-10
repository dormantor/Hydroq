#include "Helper.h"
#include "Node.h"
#include "HydMap.h"
#include "MultiSelection.h"

using namespace Cog;

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
	else if (name.compare("rig_platform") == 0) {
		return MapNodeType::RIG_PLATFORM;
	}

	return MapNodeType::NONE;
}

 string Helper::GetMapNameByNodeType(MapNodeType type) {
	 switch (type) {
	 case MapNodeType::GROUND:
		 return "ground";
	 case MapNodeType::RIG:
		 return "rig_topleft";
	 case MapNodeType::WATER:
		 return "water";
	 case MapNodeType::RIG_PLATFORM:
		 return "rig_platform";
	 }

	 throw IllegalArgumentException("Unknown map node type");
 }

 void Helper::SetPanelItem(Node* owner, Node* listNode, int index, StringHash selectionGroup, StringHash idAttr, string idValue, bool select) {

	 Node* plane = new Node("plane");
	 plane->AddBehavior(new HitEvent());

	 plane->AddBehavior(new MultiSelection(StringToColor("0x00000000"), StringToColor("0xFFFFFF88"), selectionGroup));
	 if (select) plane->SetState(StringHash(STATES_SELECTED));
	 plane->AddAttr(idAttr, idValue);
	 auto shape = spt<Plane>(new Plane(1, 1));
	 shape->SetColor(StringToColor("0x00000000"));
	 plane->SetShape(shape);

	 auto sceneSettings = owner->GetScene()->GetSettings();
	 int gridWidth = sceneSettings.GetSettingValInt("transform", "grid_width");
	 int gridHeight = sceneSettings.GetSettingValInt("transform", "grid_height");

	 TransformEnt transformEnt = TransformEnt();
	 transformEnt.pos = ofVec2f(0.05f, 0.22f + 0.18f*index);
	 transformEnt.pType = CalcType::PER;
	 transformEnt.size = ofVec2f(17, 3);
	 transformEnt.sType = CalcType::GRID;
	 TransformMath math = TransformMath();
	 math.SetTransform(plane, listNode, transformEnt, gridWidth, gridHeight);

	 listNode->AddChild(plane);


	 Node* text = new Node("text");
	 auto shape2 = spt<Text>(new Text(CogGetFont("MotionControl-Bold.otf", 35), idValue));
	 shape2->SetColor(StringToColor("0xFFFFFF"));
	 text->SetShape(shape2);

	 transformEnt = TransformEnt();
	 transformEnt.pos = ofVec2f(0.1f, 0.22f + 0.04f + 0.18f*index);
	 transformEnt.pType = CalcType::PER;
	 transformEnt.size = ofVec2f(1);
	 transformEnt.anchor = ofVec2f(0);
	 math.SetTransform(text, listNode, transformEnt, gridWidth, gridHeight);

	 listNode->AddChild(text);

 }