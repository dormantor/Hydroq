
#include "SeedBed.h"
#include "HydroqGameModel.h"

void Seedbed::Init() {
	frequencySh = StringHash(ATTR_SEEDBED_FREQUENCY);
	lastSpawnSh = StringHash(ATTR_SEEDBED_LASTSPAWN);

	// set spawn frequency
	if (!owner->HasAttr(frequencySh)) {
		owner->AddAttr(frequencySh, 0.3f);
	}

	if (!owner->HasAttr(lastSpawnSh)) {
		owner->AddAttr(lastSpawnSh, (uint64)0);
	}
}

void Seedbed::Update(const uint64 delta, const uint64 absolute) {
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
			auto gameModel = GETCOMPONENT(HydroqGameModel);
			auto position = owner->GetTransform().localPos + 0.5f;
			gameModel->SpawnWorker(position);
			Finish();
		}
	}
}

