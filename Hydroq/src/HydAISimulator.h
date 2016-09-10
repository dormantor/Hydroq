#pragma once

#include "HydAISimulator.h"
#include "HydAIState.h"
#include "HydAIAction.h"
#include "Error.h"
#include "Simulator.h"
#include "HydroqDef.h"

using namespace Cog;

#define AGENT_AI 0
#define AGENT_PLAYER 1


/**
* Simulator for AI; takes actions and transforms states
*
* The Hydroq simulator doesn't work with units nor with map; it only knows distances
* of paths between rigs and his decisions are transformed to list of tasks for units 
*/
class HydAISimulator : public Simulator<HydAIState, HydAIAction>
{
public:
	Faction aiFaction;

	HydAISimulator();

	void InitState() {
		throw IllegalOperationException("This simulator can't be reinitialized");
	}

	spt<Simulator> DeepCopy() {
		auto copy = spt<HydAISimulator>(new HydAISimulator());
		copy->actualState = this->actualState;
		copy->agentsNumber = this->agentsNumber;
		copy->possibleActions = this->possibleActions;
		copy->rewards = this->rewards;
		copy->aiFaction = this->aiFaction;
		return copy;
	}

	/**
	* Returns true, if the enemy faction is a blue faction
	*/
	bool IsBlueEnemy();

	void SetRewards(int blueReward, int redReward);

	void MakeAction(HydAIAction act);

protected:


	virtual void RecalcPossibleActions();
};
