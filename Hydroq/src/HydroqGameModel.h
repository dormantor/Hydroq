#pragma once

#include "Component.h"
#include "ofxSQLite.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"

/**
* Hydroq game model
*/
class HydroqGameModel : public Component {

	OBJECT(HydroqGameModel)

private:
	// static objects (including buildings)
	HydMap* hydroqMap;
	// dynamic objects (including buildings)
	map<Vec2i, spt<HydEntity>> mapObjects;

public:

	~HydroqGameModel() {

	}

	void Init() {
		hydroqMap = new HydMap();
	}

	void Init(spt<ofxXml> xml) {
		hydroqMap = new HydMap();
	}

	HydMap* GetMap() {
		return hydroqMap;
	}
};