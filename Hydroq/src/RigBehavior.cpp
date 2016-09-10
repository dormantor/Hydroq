
#include "RigBehavior.h"
#include "GameModel.h"



void RigBehavior::OnStart() {
	frequencySh = StrId(ATTR_SEEDBED_FREQUENCY);
	lastSpawnSh = StrId(ATTR_SEEDBED_LASTSPAWN);

	// set spawn frequency
	if (!owner->HasAttr(frequencySh)) {
		owner->AddAttr(frequencySh, 0.3f);
	}

	if (!owner->HasAttr(lastSpawnSh)) {
		owner->AddAttr(lastSpawnSh, (uint64)0);
	}
}

void RigBehavior::Update(const uint64 delta, const uint64 absolute) {
	// spawns per second
	float frequency = owner->GetAttr<float>(frequencySh);
	uint64 lastSpawn = owner->GetAttr<uint64>(lastSpawnSh);

	if (lastSpawn == 0) {
		// set initial value of the last spawn
		owner->ChangeAttr(lastSpawnSh, absolute);
	}
	else {
		int spawnDelay = absolute - lastSpawn;

		float spawnDelayRat = (spawnDelay)/1000.0f;
		if (spawnDelayRat > 1.0f/frequency) {
			// spawn a worker
			owner->ChangeAttr(lastSpawnSh, absolute);

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

			Faction faction = owner->GetAttr<Faction>(ATTR_FACTION);
			gameModel->SpawnWorker(ofVec2f(posX, posY),faction,0, Vec2i(owner->GetTransform().localPos.x, owner->GetTransform().localPos.y));

			if (totalWorkers++ > 20) Finish();
		}
	}
}

