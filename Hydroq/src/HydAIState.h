#pragma once

#include "AIState.h"
#include "Vec2i.h"
#include "Utils.h"

/**
* AI state that simplifies real game state
*/
class HydAIState : public AIState {
public:
	// distances of all red rigs
	vector<int> distancesRed;
	// distances of all blue rigs
	vector<int> distancesBlue;
	// distances of all empty rigs from blue perspective
	vector<int> distancesBlueEmpty;
	// distances of all empty rigs from red perspective
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

	int GetBlueRigsNum() {
		return distancesBlue.size();
	}

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

