#pragma once

#include "Component.h"
#include "ofxSQLite.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"
#include "HydroqGameModel.h"
#include "HydroqSpriteManager.h"

/**
* Hydroq game view
*/
class HydroqGameView : public Component {

	OBJECT(HydroqGameView)

private:
	spt<SpritesShape> staticSprites;
	spt<SpritesShape> dynamicSprites;

public:

	~HydroqGameView() {

	}

	void Init() {
		
	}

	void Init(spt<ofxXml> xml) {
		
	}

	void LoadSprites() {
		auto gameModel = GETCOMPONENT(HydroqGameModel);
		auto map = gameModel->GetMap();
		auto spriteManager = GETCOMPONENT(HydroqSpriteManager);
		auto cache = GETCOMPONENT(ResourceCache);

		auto spriteSheet = cache->GetSpriteSheet("game_board");
		auto spriteSet = spriteSheet->GetDefaultSpriteSet();
		auto mapSprites = vector<spt<SpriteEntity>>();

		for (int i = 0; i < map->GetWidth(); i++) {
			for (int j = 0; j < map->GetHeight(); j++) {
				auto obj = map->GetNode(i, j);
				auto sprite = spriteManager->GetSprite(obj->mapNodeName);

				Trans transform = Trans();
				transform.localPos.x = spriteSet->GetSpriteWidth() * i;
				transform.localPos.y = spriteSet->GetSpriteHeight() * j;

				mapSprites.push_back(spt<SpriteEntity>(new SpriteEntity(sprite, transform)));
			}
		}

		staticSprites = spt<SpritesShape>(new SpritesShape("map_board", mapSprites));

		// no dynamic sprite at beginning
		auto crates = vector<spt<SpriteEntity>>();
		dynamicSprites = spt<SpritesShape>(new SpritesShape("object_board", crates));
	}

	spt<SpritesShape> GetStaticSprites() {
		return staticSprites;
	}

	spt<SpritesShape> GetDynamicSprites() {
		return dynamicSprites;
	}
};