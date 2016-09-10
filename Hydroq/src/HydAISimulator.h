#pragma once

#include "Simulator.h"
#include "HydAIState.h"
#include "HydAIACtion.h"
#include "Error.h"
#include "Definitions.h"

#define AGENT_AI 0
#define AGENT_PLAYER 1

/**
* Simulator for AI; takes actions and transforms states
*/
class HydAISimulator : public Simulator<HydAIState, HydAIAction>
{
public:
	
	HydAISimulator() {
		// always two agents
		this->agentsNumber = 2;
	}

	void InitState() {
		this->actualState = HydAIState(AGENT_PLAYER);
		this->actualState.distancesBlue.push_back(5);
		this->actualState.distancesRed.push_back(5);
		this->actualState.distancesBlueEmpty.push_back(2);
		this->actualState.distancesRedEmpty.push_back(2);
		this->actualState.Recalc();

		RecalcPossibleActions();
		this->rewards = AgentsReward(0, 0);
	}

	spt<Simulator> DeepCopy() {
		auto copy = spt<HydAISimulator>(new HydAISimulator());
		copy->actualState = this->actualState;
		copy->agentsNumber = this->agentsNumber;
		copy->possibleActions = this->possibleActions;
		copy->rewards = this->rewards;
		return copy;
	}

	void MakeAction(HydAIAction act, bool isSimulation) {
		if (find(this->possibleActions.begin(), this->possibleActions.end(), act) == this->possibleActions.end()) {
			throw IllegalOperationException("Wrong action to take!");
		}

		int index = act.index;

		if (act.type == HydAIActionType::CAPTURE_ENEMY_RIG) {
			if (actualState.GetAgentOnTurn() == AGENT_AI) {
				if (!isSimulation) COGLOGDEBUG("AI", "Red absorbed blue rig at index %d", index);
				actualState.RemoveBlueRig(index);
				actualState.distancesRed.push_back(1);
				rewards = AgentsReward(10, 0);
			}
			else {
				if (!isSimulation) COGLOGDEBUG("AI", "Blue absorbed red rig at index %d", index);
				actualState.RemoveRedRig(index);
				actualState.distancesBlue.push_back(1);
				rewards = AgentsReward(0, 10);
			}
		}
		else if (act.type == HydAIActionType::CAPTURE_EMPTY_RIG) {
			if (actualState.GetAgentOnTurn() == AGENT_AI) {
				if (!isSimulation) COGLOGDEBUG("AI", "Red absorbed empty rig at index %d", index);
				actualState.distancesRed.push_back(actualState.distancesRedEmpty[index]);
				actualState.RemoveEmptyRig(index);
				rewards = AgentsReward(5, 0);
			}
			else {
				if (!isSimulation) COGLOGDEBUG("AI", "Blue absorbed empty rig at index %d", index);
				actualState.distancesBlue.push_back(actualState.distancesBlueEmpty[index]);
				actualState.RemoveEmptyRig(index);
				rewards = AgentsReward(0, 5);
			}
		}
		else if (act.type == HydAIActionType::GO_TO_EMPTY_RIG) {
			if (actualState.GetAgentOnTurn() == AGENT_AI) {
				if (!isSimulation) COGLOGDEBUG("AI", "Red goes to empty rig at index %d", index);
				actualState.distancesRedEmpty[index]--;
				rewards = AgentsReward(2, 0);
			}
			else {
				if (!isSimulation) COGLOGDEBUG("AI", "Blue goes to empty rig at index %d",index);
				actualState.distancesBlueEmpty[index]--;
				rewards = AgentsReward(0, 2);
			}
		}
		else if (act.type == HydAIActionType::GO_TO_ENEMY_RIG) {
			if (actualState.GetAgentOnTurn() == AGENT_AI) {
				if (!isSimulation) COGLOGDEBUG("AI", "Red goes to blue rig at index %d",index);
				actualState.distancesBlue[index]--;
				rewards = AgentsReward(1, 0);
			}
			else {
				if (!isSimulation) COGLOGDEBUG("AI", "Blue goes to red rig at index %d",index);
				actualState.distancesRed[index]--;
				rewards = AgentsReward(0, 1);
			}
		}

		if (!isSimulation) {
			COGLOGDEBUG("AI", actualState.WriteInfo().c_str());
		}

		this->actualState.SwapAgentOnTurn();
		this->actualState.Recalc();
		RecalcPossibleActions();
	}

protected:


	virtual void RecalcPossibleActions() {
		this->possibleActions.clear();

		if (actualState.distancesBlue.empty() || actualState.distancesRed.empty()) return;

		auto& distancesToEnemy = actualState.GetAgentOnTurn() == AGENT_AI ? actualState.distancesBlue : actualState.distancesRed;
		auto& distancesToEmpty = actualState.GetAgentOnTurn() == AGENT_AI ? actualState.distancesRedEmpty : actualState.distancesBlueEmpty;

		for (int i = 0; i < distancesToEnemy.size(); i++) {
			if (distancesToEnemy[i] == 0) {
				// zero distance -> rig can be captured
				possibleActions.push_back(HydAIAction(HydAIActionType::CAPTURE_ENEMY_RIG, i));
			}
			else {
				// non-zero distance
				possibleActions.push_back(HydAIAction(HydAIActionType::GO_TO_ENEMY_RIG, i));
			}
		}

		for (int i = 0; i < distancesToEmpty.size(); i++) {
			if (distancesToEmpty[i] == 0) {
				possibleActions.push_back(HydAIAction(HydAIActionType::CAPTURE_EMPTY_RIG, i));
			}
			else {
				possibleActions.push_back(HydAIAction(HydAIActionType::GO_TO_EMPTY_RIG, i));
			}
		}

	}

};
