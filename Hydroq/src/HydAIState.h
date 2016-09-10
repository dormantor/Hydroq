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

	HydAIState(int agentOnTurn) {
		this->agentOnTurn = agentOnTurn;
	}

	HydAIState(const HydAIState& copy) {
		this->distancesRed = copy.distancesRed;
		this->distancesBlue = copy.distancesBlue;
		this->distancesBlueEmpty = copy.distancesBlueEmpty;
		this->distancesRedEmpty = copy.distancesRedEmpty;
		this->agentOnTurn = copy.agentOnTurn;
	}

	void RemoveRedRig(int index) {
		int counter = 0;
		for (auto it = distancesRed.begin(); it != distancesRed.end(); ++it) {
			if (counter++ == index) {
				distancesRed.erase(it);
				break;
			}
		}
	}

	void RemoveBlueRig(int index) {
		int counter = 0;
		for (auto it = distancesBlue.begin(); it != distancesBlue.end(); ++it) {
			if (counter++ == index) {
				distancesBlue.erase(it);
				break;
			}
		}
	}

	void RemoveEmptyRig(int index) {
		int counter = 0;
		for (auto it = distancesBlueEmpty.begin(); it != distancesBlueEmpty.end(); ++it) {
			if (counter++ == index) {
				distancesBlueEmpty.erase(it);
				break;
			}
		}

		counter = 0;
		for (auto it = distancesRedEmpty.begin(); it != distancesRedEmpty.end(); ++it) {
			if (counter++ == index) {
				distancesRedEmpty.erase(it);
				break;
			}
		}
	}

	string WriteInfo() {
		
		ostringstream ss;
		ss << "BLUE-RED dist: ";
		for (auto dist : distancesRed) {
			ss << dist << ", ";
		}
		ss << endl;

		ss << "RED-BLUE dist: ";
		for (auto dist : distancesBlue) {
			ss << dist << ", ";
		}
		ss << endl;
		ss << "BLUE-EMPTY dist: ";
		for (auto dist : distancesBlueEmpty) {
			ss << dist << ", ";
		}
		ss << endl;
		ss << "RED-EMPTY dist: ";
		for (auto dist : distancesRedEmpty) {
			ss << dist << ", ";
		}
		ss << endl;

		return ss.str();
	}

	void SwapAgentOnTurn() {
		this->agentOnTurn = (agentOnTurn == 0) ? 1 : 0;
	}

	int GetBlueRigsNum() {
		return distancesBlue.size();
	}

	int GetRedRigsNum() {
		return distancesRed.size();
	}

	void Recalc() {
		sort(distancesRed.begin(), distancesRed.end(),
			[](const int & a, const int & b) -> bool
		{
			return a > b;
		});
		sort(distancesBlue.begin(), distancesBlue.end(),
			[](const int & a, const int & b) -> bool
		{
			return a > b;
		});
		sort(distancesRedEmpty.begin(), distancesRedEmpty.end(),
			[](const int & a, const int & b) -> bool
		{
			return a > b;
		});

		sort(distancesBlueEmpty.begin(), distancesBlueEmpty.end(),
			[](const int & a, const int & b) -> bool
		{
			return a > b;
		});
	}

	bool operator ==(const HydAIState& rhs) const
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
	}

	bool operator !=(const HydAIState& rhs) const { return !(*this == rhs); }


};

