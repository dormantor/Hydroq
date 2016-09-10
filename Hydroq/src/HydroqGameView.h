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

	// dynamic sprites are not positioned
	spt<SpritesShape> dynamicSprites;
	map<Vec2i, spt<SpriteEntity>> dynamicSpriteMap;
	HydroqSpriteManager* spriteManager;
	spt<SpriteSet> defaultSpriteSet;

public:

	~HydroqGameView() {

	}

	void Init() {
		spriteManager = GETCOMPONENT(HydroqSpriteManager);
		RegisterGlobalListening(ACT_MAP_OBJECT_CHANGED);
	}

	void Init(spt<ofxXml> xml) {
		Init();
	}

	void OnMessage(Msg& msg) {
		if (msg.GetAction() == StringHash(ACT_MAP_OBJECT_CHANGED)) {
			MapObjectChangedEvent* evt = static_cast<MapObjectChangedEvent*>(msg.GetData());

			if (evt->changeType == ObjectChangeType::DYNAMIC_CHANGED) {
				// create new sprite
				auto sprite = spriteManager->GetSprite(evt->changedEntity->GetName());
				Trans transform = Trans();
				transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * evt->changedNode->pos.x;
				transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * evt->changedNode->pos.y;
				dynamicSprites->GetSprites().push_back(spt<SpriteEntity>(new SpriteEntity(sprite, transform)));
			}
		}
	}

	void LoadSprites() {
		auto gameModel = GETCOMPONENT(HydroqGameModel);
		auto map = gameModel->GetMap();
		auto cache = GETCOMPONENT(ResourceCache);

		auto spriteSheet = cache->GetSpriteSheet("game_board");
		defaultSpriteSet = spriteSheet->GetDefaultSpriteSet();
		auto mapSprites = vector<spt<SpriteEntity>>();

		for (int i = 0; i < map->GetWidth(); i++) {
			for (int j = 0; j < map->GetHeight(); j++) {
				auto obj = map->GetNode(i, j);
				auto sprite = spriteManager->GetSprite(obj->mapNodeName);

				Trans transform = Trans();
				transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * i;
				transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * j;

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