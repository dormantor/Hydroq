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
	Faction aiFaction;

	HydAISimulator() {
		// always two agents
		this->agentsNumber = 2;
		this->rewards = AgentsReward(0, 0);
	}

	void InitState() {
		throw IllegalOperationException("This simulator can't be reinitialized");
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

		if (act.type == HydAIActionType::CAPTURE_ENEMY) {
			if (actualState.GetAgentOnTurn() == AGENT_AI && aiFaction == Faction::RED) {
				actualState.RemoveBlueRig(index);
				actualState.distancesRed.push_back(1);
				rewards = AgentsReward(100, 0);
			}
			else {
				actualState.RemoveRedRig(index);
				actualState.distancesBlue.push_back(1);
				rewards = AgentsReward(0, 100);
			}
		}
		else if (act.type == HydAIActionType::CAPTURE_EMPTY) {
			if (actualState.GetAgentOnTurn() == AGENT_AI && aiFaction == Faction::RED) {
				actualState.distancesRed.push_back(actualState.distancesRedEmpty[index]);
				actualState.RemoveEmptyRig(index);
				rewards = AgentsReward(50, 0);
			}
			else {
				actualState.distancesBlue.push_back(actualState.distancesBlueEmpty[index]);
				actualState.RemoveEmptyRig(index);
				rewards = AgentsReward(0, 50);
			}
		}
		else if (act.type == HydAIActionType::GOTO_EMPTY) {
			if (actualState.GetAgentOnTurn() == AGENT_AI && aiFaction == Faction::RED) {
				actualState.distancesRedEmpty[index]--;
				rewards = AgentsReward(2, 0);
			}
			else {
				actualState.distancesBlueEmpty[index]--;
				rewards = AgentsReward(0, 2);
			}
		}
		else if (act.type == HydAIActionType::GOTO_ENEMY) {
			if (actualState.GetAgentOnTurn() == AGENT_AI && aiFaction == Faction::RED) {
				actualState.distancesBlue[index]--;
				rewards = AgentsReward(1, 0);
			}
			else {
				actualState.distancesRed[index]--;
				rewards = AgentsReward(0, 1);
			}
		}

		this->actualState.Recalc();
		this->actualState.SwapAgentOnTurn(this->agentsNumber);
		RecalcPossibleActions();
	}

protected:


	virtual void RecalcPossibleActions() {
		this->possibleActions.clear();

		if (actualState.distancesBlue.empty() || actualState.distancesRed.empty()) return;

		auto& distancesToEnemy = (aiFaction == Faction::RED && actualState.GetAgentOnTurn() == AGENT_AI) ? actualState.distancesBlue : actualState.distancesRed;
		auto& distancesToEmpty = (aiFaction == Faction::RED && actualState.GetAgentOnTurn() == AGENT_AI) ? actualState.distancesRedEmpty : actualState.distancesBlueEmpty;

		for (int i = 0; i < distancesToEnemy.size(); i++) {
			if (distancesToEnemy[i] == 0) {
				// zero distance -> rig can be captured
				possibleActions.push_back(HydAIAction(HydAIActionType::CAPTURE_ENEMY, i));
			}
			else {
				// non-zero distance
				possibleActions.push_back(HydAIAction(HydAIActionType::GOTO_ENEMY, i));
			}
		}

		for (int i = 0; i < distancesToEmpty.size(); i++) {
			if (distancesToEmpty[i] == 0) {
				possibleActions.push_back(HydAIAction(HydAIActionType::CAPTURE_EMPTY, i));
			}
			else {
				possibleActions.push_back(HydAIAction(HydAIActionType::GOTO_EMPTY, i));
			}
		}
	}
};
