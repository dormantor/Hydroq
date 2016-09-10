#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class Seedbed : public Behavior {
	OBJECT_PROTOTYPE(Seedbed)

	StringHash frequencySh;
	StringHash lastSpawnSh;

	void Init() {
		frequencySh = StringHash(ATTR_SEEDBED_FREQUENCY);
		lastSpawnSh = StringHash(ATTR_SEEDBED_LASTSPAWN);

		// set spawn frequency
		if (!owner->HasAttr(frequencySh)) {
			owner->AddAttr(frequencySh, 0.1f);
		}

		if (!owner->HasAttr(lastSpawnSh)) {
			owner->AddAttr(lastSpawnSh, (uint64)0);
		}
	}

	void OnMessage(Msg& msg) {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute);
};

