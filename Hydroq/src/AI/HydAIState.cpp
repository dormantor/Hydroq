#include "HydAIState.h"

HydAIState::HydAIState(int agentOnTurn) {
	this->agentOnTurn = agentOnTurn;
}

HydAIState::HydAIState(const HydAIState& copy) {
	this->distancesRed = copy.distancesRed;
	this->distancesBlue = copy.distancesBlue;
	this->distancesBlueEmpty = copy.distancesBlueEmpty;
	this->distancesRedEmpty = copy.distancesRedEmpty;
	this->agentOnTurn = copy.agentOnTurn;
}


void HydAIState::RemoveFromArray(int index, vector<int>& ar) {
	int counter = 0;
	for (auto it = ar.begin(); it != ar.end(); ++it) {
		if (counter++ == index) {
			ar.erase(it);
			break;
		}
	}
}


void HydAIState::Recalc() {
	// sort all distances from the nearest
	sort(distancesRed.begin(), distancesRed.end(),
		[](const int & a, const int & b) -> bool
	{
		return a < b;
	});
	sort(distancesBlue.begin(), distancesBlue.end(),
		[](const int & a, const int & b) -> bool
	{
		return a < b;
	});
	sort(distancesRedEmpty.begin(), distancesRedEmpty.end(),
		[](const int & a, const int & b) -> bool
	{
		return a < b;
	});

	sort(distancesBlueEmpty.begin(), distancesBlueEmpty.end(),
		[](const int & a, const int & b) -> bool
	{
		return a < b;
	});
}

bool HydAIState::operator ==(const HydAIState& rhs) const
{
	for (int i = 0; i < distancesRed.size(); i++) {
		if (distancesRed[i] != rhs.distancesRed[i]) return false;
	}

	for (int i = 0; i < distancesBlue.size(); i++) {
		if (distancesBlue[i] != rhs.distancesBlue[i]) return false;
	}

	for (int i = 0; i < distancesRedEmpty.size(); i++) {
		if (distancesRedEmpty[i] != rhs.distancesRedEmpty[i]) return false;
	}

	for (int i = 0; i < distancesBlueEmpty.size(); i++) {
		if (distancesBlueEmpty[i] != rhs.distancesBlueEmpty[i]) return false;
	}

	return true;
}
