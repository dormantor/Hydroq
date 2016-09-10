#include "MultiplayerMenu.h"
#include "Node.h"
#include "MultiSelection.h"
#include "TransformMath.h"

void MultiplayerMenu::AddServer(string ip) {

	int index = foundIps.size() - 1;

	Node* hosts_list = owner->GetScene()->FindNodeByTag("hosts_list");

	Node* plane = new Node("plane");
	plane->AddBehavior(new HitEvent());

	plane->AddBehavior(new MultiSelection(StringToColor("0x00000000"), StringToColor("0xFFFFFF88"), StringHash("SELECTION_SERVER")));
	plane->AddAttr(ATTR_SERVER_IP, ip);
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
	math.SetTransform(plane, hosts_list, transformEnt, gridWidth, gridHeight);

	hosts_list->AddChild(plane);


	Node* text = new Node("text");
	auto shape2 = spt<Text>(new Text(CogGetFont("MotionControl-Bold.otf", 35), ip));
	shape2->SetColor(StringToColor("0xFFFFFF"));
	text->SetShape(shape2);

	transformEnt = TransformEnt();
	transformEnt.pos = ofVec2f(0.1f, 0.22f + 0.04f + 0.18f*index);
	transformEnt.pType = CalcType::PER;
	transformEnt.size = ofVec2f(1);
	transformEnt.anchor = ofVec2f(0);
	math.SetTransform(text, hosts_list, transformEnt, gridWidth, gridHeight);

	hosts_list->AddChild(text);

}

void MultiplayerMenu::RefreshServers() {
	Node* hosts_list = owner->GetScene()->FindNodeByTag("hosts_list");

	// remove all textboxes and refresh them
	for (auto& child : hosts_list->GetChildren()) {
		hosts_list->RemoveChild(child, true);
	}

	for (auto& ip : foundIps) {
		AddServer(ip);
	}

	// disable CONNECT button
	owner->GetScene()->FindNodeByTag("connect_but")->SetState(StringHash(STATES_DISABLED));
}