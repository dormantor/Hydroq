
#include "RigBehavior.h"
#include "GameModel.h"
#include "Node.h"
#include "ComponentStorage.h"

void RigBehavior::OnInit() {
	auto& settings = CogGetProjectSettings();
	this->rigCapacity = settings.GetSettingValInt("hydroq_set", "rig_capacity");
}

void RigBehavior::Update(const uint64 delta, const uint64 absolute) {
	
	if (lastSpawn == 0) {
		lastSpawn = absolute+3000;
	}
	else {
		

		if (IsProperTime(lastSpawn,absolute, spawnFrequency)) {
		
			// spawn a worker
			lastSpawn = absolute;
			
			// set starting position as a random position near
			// the drilling rig
			float circuitPosition = ofRandom(0, 8);
			float posX = 0;
			float posY = 0;
			auto thisPos = owner->GetTransform().localPos;

			if (circuitPosition <= 2) {
				posX = thisPos.x + circuitPosition;
				posY = thisPos.y - 0.25f;
			}
			else if (circuitPosition <= 4) {
				posX = thisPos.x + 2 + 0.25f;
				posY = thisPos.y + (circuitPosition - 2);
			}
			else if (circuitPosition <= 6) {
				posX = thisPos.x + (6 - circuitPosition);
				posY = thisPos.y + 2 + 0.25f;
			}
			else {
				posX = thisPos.x - 0.25f;
				posY = thisPos.y + (8 - circuitPosition);
			}

			// spawn worker
			Faction faction = owner->GetAttr<Faction>(ATTR_FACTION);
			auto playerModel = GETCOMPONENT(PlayerModel);
			if (faction == playerModel->GetFaction() || (!playerModel->IsMultiplayer() && faction != Faction::NONE)) {
				gameModel->SpawnWorker(ofVec2f(posX, posY), faction, 0, Vec2i(owner->GetTransform().localPos.x, owner->GetTransform().localPos.y));

				if (totalWorkers++ > rigCapacity) Finish();
			}
			
		}
	}
}

