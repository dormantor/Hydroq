#include "HydroqGameView.h"
#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"
#include "HydroqGameModel.h"


void HydroqGameView::Init() {
	RegisterGlobalListening(ACT_MAP_OBJECT_CHANGED);
}


void HydroqGameView::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_MAP_OBJECT_CHANGED)) {		
		MapObjectChangedEvent* evt = static_cast<MapObjectChangedEvent*>(msg.GetData());

		if (evt->changeType == ObjectChangeType::DYNAMIC_CREATED || evt->changeType == ObjectChangeType::MOVING_CREATED) {
			// new dynamic or moving object
			auto trans = evt->changedNode->GetTransform();
			// create new sprite
			auto sprite = spriteTypes[evt->changedNode->GetTag()];

			Trans transform = Trans();
			// moving objects will have its origin set to the middle of the cell
			if (evt->changeType == ObjectChangeType::MOVING_CREATED) {
				transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * trans.localPos.x - defaultSpriteSet->GetSpriteWidth() / 2;
				transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * trans.localPos.y - defaultSpriteSet->GetSpriteHeight() / 2;
			}
			else {
				transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * trans.localPos.x;
				transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * trans.localPos.y;
			}

			transform.localPos.z = trans.localPos.z; // set z-index

			// push into collection of dynamic sprites
			auto newEntity = spt<SpriteEntity>(new SpriteEntity(sprite, transform));
			dynamicSprites->GetSprites().push_back(newEntity);
			dynamicSprites->RefreshZIndex();
			dynamicSpriteEntities[evt->changedNode->GetId()] = newEntity;
		}
		else if (evt->changeType == ObjectChangeType::DYNAMIC_REMOVED || evt->changeType == ObjectChangeType::MOVING_REMOVED) {

			// dynamic or moving object removed
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
			auto sprite = spriteTypes[mapNode->mapNodeName];
			sprEntity->sprite = sprite;
		}
	}
}

void HydroqGameView::LoadSprites(Setting sprites) {
	auto gameModel = GETCOMPONENT(HydroqGameModel);
	auto map = gameModel->GetMap();
	auto cache = GETCOMPONENT(ResourceCache);
	auto spriteSheet = cache->GetSpriteSheet("game_board");
	defaultSpriteSet = spriteSheet->GetDefaultSpriteSet();
	auto mapSprites = vector<spt<SpriteEntity>>();

	// load all sprite types
	for (auto& it : sprites.items) {
		int index = it.second.GetValInt();
		string name = it.second.key;
		spriteTypes[name] = spt<Sprite>(new Sprite(defaultSpriteSet, index));
	}

	// load all sprites on the map
	for (int i = 0; i < map->GetWidth(); i++) {
		for (int j = 0; j < map->GetHeight(); j++) {
			auto obj = map->GetNode(i, j);
			
			spt<Sprite> sprite;
			
			if (obj->owner == Faction::BLUE && obj->mapNodeType == MapNodeType::RIG) {
				sprite = spt<Sprite>(new Sprite(defaultSpriteSet, spriteTypes[obj->mapNodeName]->GetFrame()+4));
			}
			else if (obj->owner == Faction::RED && obj->mapNodeType == MapNodeType::RIG) {
				sprite = spt<Sprite>(new Sprite(defaultSpriteSet, spriteTypes[obj->mapNodeName]->GetFrame() + 8));
			}
			else {
				sprite = spriteTypes[obj->mapNodeName];
			}

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


void HydroqGameView::Update(const uint64 delta, const uint64 absolute) {

	auto model = GETCOMPONENT(HydroqGameModel);
	auto& movingObjects = model->GetMovingObjects();

	StringHash stateIdle = StringHash(STATE_WORKER_IDLE);
	StringHash stateBuild = StringHash(STATE_WORKER_BUILD);

	// update transformation of all objects
	for (auto& dynObj : movingObjects) {
		int id = dynObj->GetId();
		auto sprite = dynamicSpriteEntities[id];

		sprite->transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * dynObj->GetTransform().localPos.x - defaultSpriteSet->GetSpriteWidth() / 2;
		sprite->transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * dynObj->GetTransform().localPos.y - defaultSpriteSet->GetSpriteHeight() / 2;
		sprite->transform.rotation = dynObj->GetTransform().rotation;

		if (dynObj->HasState(stateIdle)) {
			// set image according to state
			sprite->sprite = spriteTypes[dynObj->GetTag()]; 
		}
		else if (dynObj->HasState(stateBuild)) {
			// set animation
			int frame = spriteTypes[dynObj->GetTag()]->GetFrame();
			if (CogGetFrameCounter() % 5 == 0) frame += 1;
			else frame += 2;

			sprite->sprite = spt<Sprite>(new Sprite(defaultSpriteSet, frame));
		}
	}
}

void HydroqGameView::SaveMapImageToFile(string file){
	// ============ uncomment this if you want to generate map image for detail window ===============
	auto gameModel = GETCOMPONENT(HydroqGameModel);
	auto cache = GETCOMPONENT(ResourceCache);
	auto spriteSheet = cache->GetSpriteSheet("game_board");
	MapLoader mapLoader;
	Settings mapConfig = Settings();
	auto xml = CogLoadXMLFile("mapconfig.xml");
	xml->pushTag("settings");
	mapConfig.LoadFromXml(xml);
	xml->popTag();
	string mapPath = mapConfig.GetSettingVal("maps_files", gameModel->GetMapName());
	auto bricks = mapLoader.LoadFromPNGImage(mapPath, mapConfig.GetSetting("names"));
	mapLoader.GenerateMapImage(bricks, spriteTypes, spriteSheet, file, 0.1f);

	// ===============================================================================================
}