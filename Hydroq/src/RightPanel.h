#pragma once

#include "Behavior.h"
#include "Definitions.h"

namespace Cog {
	class Scene;
}

using namespace Cog;

/**
* Right panel section
*/
enum class Section {
	NONE, 
	BUILD,		/** building actions */
	COMMAND		/** commands (attractor) */
};

/**
* Behavior that controls the right panel on the game board
*/
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

	/**
	* Selects the section for building
	*/
	void SelectBuildSection();

	/**
	* Selects the section for commands
	*/
	void SelectCommandSection();

	void UnSelectNodes();

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};