#pragma once

#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"

class HydroqSpriteManager : public Component {

	OBJECT(HydroqSpriteManager)

private:
	map<string, spt<Sprite>> allSprites;

public:

	~HydroqSpriteManager() {

	}

	void Init() {
		
	}

	void Init(spt<ofxXml> xml) {
		
	}

	void LoadSprites(Setting sprites) {
		auto cache = GETCOMPONENT(ResourceCache);
		auto spriteSheet = cache->GetSpriteSheet("game_board");
		auto spriteSet = spriteSheet->GetDefaultSpriteSet();

		for (auto& it : sprites.items) {
			int index = it.second.GetValInt();
			string name = it.second.key;
			allSprites[name] = spt<Sprite>(new Sprite(spriteSet, index));
		}
	}

	spt<Sprite> GetSprite(string name) {
		return allSprites[name];
	}

	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};