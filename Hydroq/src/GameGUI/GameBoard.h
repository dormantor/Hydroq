#pragma once

#include "Behavior.h"
#include "Settings.h"
#include "Transform.h"
#include "Definitions.h"
#include "Resources.h"

using namespace Cog;

/**
* Behavior that controls game board, with the ability to 
* zoom to the selected location
*/
class GameBoard : public Behavior {
	
private:
	Resources* cache;
	Settings mapConfig;

public:

	GameBoard() {

	}

	void OnInit();

	/**
	* Zooms into position
	* @param positionRelative center of position to zoom in percentage of the size of the game board
	*/
	void ZoomIntoPositionCenter(ofVec2f positionRelative);

protected:

	/** Sets new position of the game board */
	void SetNewPosition(Trans& transform, ofVec3f& newAbsPos);

	/** Checks new position of the game board if it is valid */
	void CheckNewPosition(Trans& transform, ofVec3f& newPos);

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};