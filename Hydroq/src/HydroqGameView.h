#pragma once

#include "Component.h"
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

	spt<SpriteSet> defaultSpriteSet;
	// dynamic sprites are not positioned
	spt<SpritesShape> dynamicSprites;
	map<int, spt<SpriteEntity>> dynamicSpriteEntities;
	HydroqSpriteManager* spriteManager;
	

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
			auto trans = evt->changedNode->GetTransform();

			if (evt->changeType == ObjectChangeType::DYNAMIC_CREATED || evt->changeType == ObjectChangeType::MOVING_CREATED) {
				// create new sprite
				auto sprite = spriteManager->GetSprite(evt->changedNode->GetTag());
				Trans transform = Trans();
				transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * trans.localPos.x;
				transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * trans.localPos.y;
				auto newEntity = spt<SpriteEntity>(new SpriteEntity(sprite, transform));
				dynamicSprites->GetSprites().push_back(newEntity);
				dynamicSpriteEntities[evt->changedNode->GetId()] = newEntity;
			}
			else if (evt->changeType == ObjectChangeType::DYNAMIC_REMOVED || evt->changeType == ObjectChangeType::MOVING_REMOVED) {

				auto oldEntity = dynamicSpriteEntities[evt->changedNode->GetId()];
				dynamicSpriteEntities.erase(evt->changedNode->GetId());

				// todo: performance!
				for (auto it = dynamicSprites->GetSprites().begin(); it != dynamicSprites->GetSprites().end(); ++it) {
					if ((*it)->id == oldEntity->id) {
						dynamicSprites->GetSprites().erase(it);
						break;
					}
				}
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

	virtual void Update(const uint64 delta, const uint64 absolute) {
		auto model = GETCOMPONENT(HydroqGameModel);
		auto& dynObjects = model->GetMovingObjects();

		for (auto& dynObj : dynObjects) {
			int id = dynObj->GetId();
			auto sprite = dynamicSpriteEntities[id];
			
			sprite->transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * dynObj->GetTransform().localPos.x;
			sprite->transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * dynObj->GetTransform().localPos.y;
			sprite->transform.rotation = dynObj->GetTransform().rotation;
		}
	}
};