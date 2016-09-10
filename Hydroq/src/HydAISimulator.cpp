#include "HydAISimulator.h"


HydAISimulator::HydAISimulator() {
	// always two agents
	this->agentsNumber = 2;
	this->rewards = AgentsReward(0, 0);
}

bool HydAISimulator::IsBlueEnemy() {
	return (actualState.GetAgentOnTurn() == AGENT_AI && aiFaction == Faction::RED)
		|| (actualState.GetAgentOnTurn() != AGENT_AI && aiFaction == Faction::BLUE);
}

void HydAISimulator::SetRewards(int blueReward, int redReward) {
	if (aiFaction == Faction::BLUE) {
		rewards = AgentsReward(blueReward, redReward);
	}
	else {
		rewards = AgentsReward(redReward, blueReward);
	}
}

void HydAISimulator::MakeAction(HydAIAction act) {
	if (find(this->possibleActions.begin(), this->possibleActions.end(), act) == this->possibleActions.end()) {
		throw IllegalOperationException("Wrong action to take!");
	}

	int index = act.index;

	if (act.type == HydAIActionType::CAPTURE_ENEMY) {
		if (IsBlueEnemy()) {
			actualState.RemoveBlueRig(index);
			actualState.distancesRed.push_back(1);
			SetRewards(100, 0);
		}
		else {
			actualState.RemoveRedRig(index);
			actualState.distancesBlue.push_back(1);
			SetRewards(0, 100);
		}
	}
	else if (act.type == HydAIActionType::CAPTURE_EMPTY) {
		if (IsBlueEnemy()) {
			actualState.distancesRed.push_back(actualState.distancesRedEmpty[index]);
			actualState.RemoveEmptyRig(index);
			SetRewards(50, 0);
		}
		else {
			actualState.distancesBlue.push_back(actualState.distancesBlueEmpty[index]);
			actualState.RemoveEmptyRig(index);
			SetRewards(0, 50);
		}
	}
	else if (act.type == HydAIActionType::GOTO_EMPTY) {
		if (IsBlueEnemy()) {
			actualState.distancesRedEmpty[index]--;
			SetRewards(2, 0);
		}
		else {
			actualState.distancesBlueEmpty[index]--;
			SetRewards(0, 2);
		}
	}
	else if (act.type == HydAIActionType::GOTO_ENEMY) {
		if (IsBlueEnemy()) {
			actualState.distancesBlue[index]--;
			SetRewards(1, 0);
		}
		else {
			actualState.distancesRed[index]--;
			SetRewards(0, 1);
		}
	}

	this->actualState.Recalc();
	this->actualState.SwapAgentOnTurn(this->agentsNumber);
	RecalcPossibleActions();
}

void HydAISimulator::RecalcPossibleActions() {
	this->possibleActions.clear();

	if (actualState.distancesBlue.empty() || actualState.distancesRed.empty()) return;

	auto& distancesToEnemy = (IsBlueEnemy()) ? actualState.distancesBlue : actualState.distancesRed;
	auto& distancesToEmpty = (IsBlueEnemy()) ? actualState.distancesRedEmpty : actualState.distancesBlueEmpty;

	for (auto i = 0; i < distancesToEnemy.size(); i++) {
		if (distancesToEnemy[i] == 0) {
			// zero distance -> rig can be captured
			possibleActions.push_back(HydAIAction(HydAIActionType::CAPTURE_ENEMY, i));
		}
		else {
			// non-zero distance
			possibleActions.push_back(HydAIAction(HydAIActionType::GOTO_ENEMY, i));
		}
	}

	for (auto i = 0; i < distancesToEmpty.size(); i++) {
		if (distancesToEmpty[i] == 0) {
			possibleActions.push_back(HydAIAction(HydAIActionType::CAPTURE_EMPTY, i));
		}
		else {
			possibleActions.push_back(HydAIAction(HydAIActionType::GOTO_EMPTY, i));
		}
	}
}