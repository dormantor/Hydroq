#pragma once

#include "ofxCogMain.h"

enum class Section {
	NONE, BUILD, COMMAND, OTHER
};

class RightPanel : public Behavior {
	
	Section selectedSection = Section::NONE;
	int buildIconId;
	int flagIconId;

	int buildSectionId;
	int flagSectionId;
	
	Scene* scene;

public:

	RightPanel() {

	}

	void OnInit();

	void OnStart();

	void OnMessage(Msg& msg);

	void SelectBuildSection();

	void SelectFlagSection();

	void UnSelectNodes();

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};