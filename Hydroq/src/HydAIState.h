#pragma once

#include "AIState.h"
#include "Vec2i.h"
#include "Utils.h"

/**
* AI state that simplifies real game state
*/
class HydAIState : public AIState {
public:
	// distances to all red rigs
	vector<int> distancesRed;
	// distances to all blue rigs
	vector<int> distancesBlue;
	// distances of all empty rigs from blue faction
	vector<int> distancesBlueEmpty;
	// distances of all empty rigs from red faction
	vector<int> distancesRedEmpty;

	HydAIState() {

	}

	HydAIState(int agentOnTurn);

	HydAIState(const HydAIState& copy);

	void RemoveRedRig(int index) {
		RemoveFromArray(index, distancesRed);
	}

	void RemoveBlueRig(int index) {
		RemoveFromArray(index, distancesBlue);
	}

	void RemoveEmptyRig(int index) {
		RemoveFromArray(index, distancesBlueEmpty);
		RemoveFromArray(index, distancesRedEmpty);
	}

	/**
	* Gets number of blue rigs
	*/
	int GetBlueRigsNum() {
		return distancesBlue.size();
	}

	/**
	* Gets number of red rigs
	*/
	int GetRedRigsNum() {
		return distancesRed.size();
	}

private:
	void RemoveFromArray(int index, vector<int>& ar);

	void Recalc();

	bool operator ==(const HydAIState& rhs) const;

	bool operator !=(const HydAIState& rhs) const { return !(*this == rhs); }

	friend class HydAISimulator;
};

