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
	map<Vec2i, spt<SpriteEntity>> staticSpriteMap;

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
			
			if (evt->changeType == ObjectChangeType::DYNAMIC_CREATED || evt->changeType == ObjectChangeType::MOVING_CREATED) {
				auto trans = evt->changedNode->GetTransform();
				// create new sprite
				auto sprite = spriteManager->GetSprite(evt->changedNode->GetTag());
				Trans transform = Trans();
				if (evt->changeType == ObjectChangeType::MOVING_CREATED) {
					transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * trans.localPos.x - defaultSpriteSet->GetSpriteWidth() / 2;
					transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * trans.localPos.y - defaultSpriteSet->GetSpriteHeight() / 2;
				}
				else {
					transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * trans.localPos.x;
					transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * trans.localPos.y;
				}
				
				transform.localPos.z = trans.localPos.z; // set z-index
				auto newEntity = spt<SpriteEntity>(new SpriteEntity(sprite, transform));
				dynamicSprites->GetSprites().push_back(newEntity);
				dynamicSprites->RefreshZIndex();
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
			else if (evt->changeType == ObjectChangeType::STATIC_CHANGED) {
				auto mapNode = evt->changedMapNode;
				// get sprite entity and change its frame
				spt<SpriteEntity> sprEntity = staticSpriteMap[mapNode->pos];
				auto sprite = spriteManager->GetSprite(mapNode->mapNodeName);
				sprEntity->sprite = sprite;
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
				auto sprEntity = spt<SpriteEntity>(new SpriteEntity(sprite, transform));
				mapSprites.push_back(sprEntity);
				staticSpriteMap[Vec2i(i, j)] = sprEntity;
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

			sprite->transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * dynObj->GetTransform().localPos.x - defaultSpriteSet->GetSpriteWidth() / 2;
			sprite->transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * dynObj->GetTransform().localPos.y - defaultSpriteSet->GetSpriteHeight() / 2;
			sprite->transform.rotation = dynObj->GetTransform().rotation;
		}
	}
};