#include "GameView.h"
#include "Component.h"
#include "HydroqDef.h"
#include "MsgPayloads.h"
#include "GameMap.h"
#include "GameModel.h"
#include "MultiAnim.h"
#include "SpriteSheet.h"
#include "ComponentStorage.h"
#include "Stage.h"
#include "Tween.h"
#include "TransformEnt.h"
#include "TransformMath.h"
#include "MapLoader.h"
#include "TransformAnim.h"
#include "ResourceCache.h"
#include "HydNetworkReceiver.h"

void GameView::OnInit() {
	SubscribeForMessages(ACT_MAP_OBJECT_CHANGED, ACT_SCENE_SWITCHED);

	gameModel = owner->GetBehavior<GameModel>();
	playerModel = GETCOMPONENT(PlayerModel);
	auto resCache = GETCOMPONENT(ResourceCache);
	hydroqSettings = resCache->GetProjectSettings();
}


void GameView::OnMessage(Msg& msg) {
	if (msg.HasAction(ACT_SCENE_SWITCHED) 
		&& msg.GetContextNode()->GetScene()->GetSceneType() != SceneType::DIALOG
		&& msg.GetContextNode()->GetId() != owner->GetSceneRoot()->GetId()) {
		// scene switched back -> stop all sounds
		auto sounds = CogGetPlayedSounds();
		for (auto sound : sounds) {
			sound->Stop();
		}
	}
	if (msg.HasAction(ACT_MAP_OBJECT_CHANGED)) {		
		auto evt = msg.GetData<MapObjectChangedEvent>();

		if (evt->changeType == ObjectChangeType::DYNAMIC_CREATED || evt->changeType == ObjectChangeType::MOVING_CREATED) {
			OnDynamicObjectCreated(evt);
		}
		else if (evt->changeType == ObjectChangeType::DYNAMIC_REMOVED || evt->changeType == ObjectChangeType::MOVING_REMOVED) {
			OnDynamicObjectRemoved(evt);
		}
		else if (evt->changeType == ObjectChangeType::STATIC_CHANGED) {
			OnStaticObjectChanged(evt);

		}
		else if (evt->changeType == ObjectChangeType::RIG_CAPTURED || evt->changeType == ObjectChangeType::RIG_TAKEN) {
			OnRigTaken(evt);
		}
	}
}

void GameView::LoadSprites(Setting sprites) {
	auto map = gameModel->GetMap();
	auto cache = GETCOMPONENT(ResourceCache);
	auto spriteSheet = cache->GetSpriteSheet("game_board");
	defaultSpriteSet = spriteSheet->GetDefaultSpriteSet();
	
	staticSprites = spt<MultiSpriteMesh>(new MultiSpriteMesh("map_board"));
	dynamicSprites = spt<MultiSpriteMesh>(new MultiSpriteMesh("object_board"));


	// load all sprite types
	for (auto& it : sprites.items) {
		auto indices = it.second.GetValues();
		
		auto sprites = vector<Sprite>();
		// one sprite could be on various indices
		for (auto index : indices) {
			int indexInt = ofToInt(index);
			auto sprite = Sprite(defaultSpriteSet, indexInt);
			sprites.push_back(sprite);
		}

		string name = it.second.key;
		spriteTypes[name] = sprites;
	}

	// load all sprites on the map
	for (int i = 0; i < map->GetWidth(); i++) {
		for (int j = 0; j < map->GetHeight(); j++) {
			auto obj = map->GetTile(i, j);

			Sprite& sprite = spriteTypes[obj->GetMapTileName()][obj->GetMapTileTypeIndex()];

			Trans transform = Trans();
			transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * i;
			transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * j;
			auto sprEntity = spt<SpriteInst>(new SpriteInst(sprite, transform));
			staticSprites->AddSprite(sprEntity);
			staticSpriteMap[Vec2i(i, j)] = sprEntity;
		}
	}

	// change rigs that have faction initialized
	auto& dynObjects = gameModel->GetDynamicObjects();

	for (auto& dyn : dynObjects) {
		if (dyn.second->GetTag().compare("rig") == 0) {
			Faction fact = dyn.second->GetAttr<Faction>(ATTR_FACTION);
			
			Vec2i pos = ofVec2f(dyn.second->GetTransform().localPos);
			int index = staticSpriteMap[pos]->sprite.GetFrame();

			if (fact == Faction::BLUE) {	
				staticSpriteMap[Vec2i(pos.x, pos.y)]->sprite = spriteTypes["rig_blue"][0];
				staticSpriteMap[Vec2i(pos.x+1, pos.y)]->sprite = spriteTypes["rig_blue"][1];
				staticSpriteMap[Vec2i(pos.x, pos.y+1)]->sprite = spriteTypes["rig_blue"][2];
				staticSpriteMap[Vec2i(pos.x+1, pos.y+1)]->sprite = spriteTypes["rig_blue"][3];
			}
			else if (fact == Faction::RED) {
				staticSpriteMap[Vec2i(pos.x, pos.y)]->sprite = spriteTypes["rig_red"][0];
				staticSpriteMap[Vec2i(pos.x+1, pos.y)]->sprite = spriteTypes["rig_red"][1];
				staticSpriteMap[Vec2i(pos.x, pos.y+1)]->sprite = spriteTypes["rig_red"][2];
				staticSpriteMap[Vec2i(pos.x+1, pos.y+1)]->sprite = spriteTypes["rig_red"][3];
			}
		}
	}

	// recalculate sprite entities
	staticSprites->Recalc();
	dynamicSprites->Recalc();
}

Sprite& GameView::GetSprite(int frame) {
	if (spriteBuffer.count(frame) == 0) spriteBuffer[frame] = Sprite(defaultSpriteSet, frame);
	return spriteBuffer[frame];
}

void GameView::Update(const uint64 delta, const uint64 absolute) {

	if (playerModel->GameEnded()) {
		if (gameEndedTime == 0) gameEndedTime = absolute;
		else if ((absolute - gameEndedTime) > 3000) {
			// after 3s, finish game
			auto stage = GETCOMPONENT(Stage);
			auto scene = stage->FindSceneByName("gameend_dialog");
			stage->SwitchToScene(scene, TweenDirection::NONE);
		}
		return;
	}

	if (!firstUpdate) {
		auto sound = CogGetSound("music/GameMusic1.mp3");
		sound->SetLoop(true);
		CogPlaySound(sound);
		firstUpdate = true;
	}

	UpdateMovingObjects();

	if (CogGetFrameCounter() % 5 == 0) {
		UpdateRigAnimations();
	}

	if (CogGetFrameCounter() % 3 == 0) {
		UpdateBridgeAnimations();
	}

	if (CogGetFrameCounter() % 2 == 0) {
		if (!spriteTypes["rig_platform"].empty()) {
			UpdatePlatformAnimations();
		}
	}

	if(CogGetFrameCounter() % 30 == 0){
		UpdateActionAnimations();
	}
}

void GameView::SaveMapImageToFile(string file){
	// ============ uncomment this if you want to generate map image for detail window ===============
	auto cache = GETCOMPONENT(ResourceCache);
	auto spriteSheet = cache->GetSpriteSheet("game_board");
	MapLoader mapLoader;
	Settings mapConfig = Settings();
	auto xml = CogLoadXMLFile("config/mapconfig.xml");
	xml->pushTag("settings");
	mapConfig.LoadFromXml(xml);
	xml->popTag();
	string mapPath = mapConfig.GetSettingVal("maps_files", gameModel->GetMapName());
	TileMap bricks;
	mapLoader.LoadFromPNGImage(mapPath, mapConfig.GetSetting("names"), bricks);
	mapLoader.GenerateMapImage(bricks, spriteTypes, spriteSheet, file, 0.1f);

	// ===============================================================================================
}

void GameView::CreateAnimationText(string message) {

	CogPlaySound(CogGetSound("music/Power_Up3.wav"));
	if (animNode == nullptr) {
		animNode = new Node("animnode");
		animNode->SetMesh(spt<Image>(new Image(CogPreload2DImage("button_default_click.png"))));
		animNode->GetTransform().localPos.z = 500;
		animNode->GetTransform().localPos.x = -1000;
		animNode->GetTransform().localPos.y = -1000;
		owner->GetParent()->AddChild(animNode);
		animTextNode = new Node("inner");
		animTextNode->SetMesh(spt<Text>(new Text(CogGetFont("MotionControl-Bold.otf", 50), "")));
		animNode->AddChild(animTextNode);
	}

	animTextNode->GetMesh<Text>()->SetText(message);
	TransformEnt ent;
	ent.pos = ofVec2f(0.5f);
	ent.anchor = ofVec2f(0.5f);
	ent.pType = CalcType::PER;
	TransformMath math;
	animNode->GetTransform().CalcAbsTransform(owner->GetParent()->GetTransform());
	animTextNode->GetTransform().CalcAbsTransform(animNode->GetTransform());
	animNode->GetTransform().scale = math.CalcScale(animNode, owner->GetParent(), 0.8f, 0, CalcType::PER, 0, 0);
	math.SetTransform(animTextNode, animNode, ent, 0, 0);

	spt<TransformEnt> from1 = spt<TransformEnt>(new TransformEnt());
	spt<TransformEnt> to1 = spt<TransformEnt>(new TransformEnt());
	from1->pos = ofVec2f(-0.5f);
	from1->anchor = ofVec2f(1.0f);
	from1->rotation = -60;
	from1->rotationCentroid = ofVec2f(0.5f);
	from1->size = animNode->GetTransform().scale;
	to1->pos = ofVec2f(0.5f);
	to1->pType = CalcType::PER;
	to1->anchor = ofVec2f(0.5f);
	to1->rotation = 30;
	to1->rotationCentroid = ofVec2f(0.5f);
	to1->size = animNode->GetTransform().scale;

	auto to2 = spt<TransformEnt>(new TransformEnt(*to1));
	to2->rotation = -30;

	auto to3 = spt<TransformEnt>(new TransformEnt(*to2));
	to3->rotation = 30;

	auto to4 = spt<TransformEnt>(new TransformEnt(*to3));
	to4->rotation = -30;

	auto to5 = spt<TransformEnt>(new TransformEnt(*to4));
	to5->rotation = 30;
	to5->pos = ofVec2f(1.5f, 1.5f);
	to5->anchor = ofVec2f(0, 0);

	MultiAnim* ma = new MultiAnim();
	ma->AddAnimation(new TransformAnim(from1, to1, 800, 0, false, AnimBlend::OVERLAY, EasingFunc::cosineInOut));
	ma->AddAnimation(new TransformAnim(to1, to2, 800, 0, false, AnimBlend::OVERLAY, EasingFunc::cosineInOut));
	ma->AddAnimation(new TransformAnim(to2, to3, 800, 0, false, AnimBlend::OVERLAY, EasingFunc::cosineInOut));
	ma->AddAnimation(new TransformAnim(to3, to4, 800, 0, false, AnimBlend::OVERLAY, EasingFunc::cosineInOut));
	ma->AddAnimation(new TransformAnim(to4, to5, 800, 0, false, AnimBlend::OVERLAY, EasingFunc::cosineInOut));
	animNode->AddBehavior(ma);
}

void GameView::OnDynamicObjectCreated(spt<MapObjectChangedEvent> evt) {
	// new dynamic or moving object
	auto trans = evt->changedNode->GetTransform();
	// create new sprite
	auto& sprite = spriteTypes[evt->changedNode->GetTag()][0];

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

	// skip marks of AI players if not set in config.xml
	if (evt->changedNode->GetTag().compare("bridgemark") == 0
		&& evt->changedNode->GetAttr<Faction>(ATTR_FACTION) != playerModel->GetFaction()
		&& !hydroqSettings.GetSettingValBool("hydroq_set", "place_enemy_bridge"))
	{
		return;
	}
	// push into collection of dynamic sprites
	auto newEntity = spt<SpriteInst>(new SpriteInst(sprite, transform));
	dynamicSprites->AddSprite(newEntity);
	dynamicSprites->RefreshZIndex();
	dynamicSpriteEntities[evt->changedNode->GetId()] = newEntity;
}

void GameView::OnDynamicObjectRemoved(spt<MapObjectChangedEvent> evt) {
	// dynamic or moving object removed
	auto oldEntity = dynamicSpriteEntities[evt->changedNode->GetId()];
	dynamicSpriteEntities.erase(evt->changedNode->GetId());

	dynamicSprites->RemoveSprite(oldEntity);

	if (evt->changedMapTile->GetMapTileType() == MapTileType::GROUND) {
		// add explosion effect
		Trans transform = evt->changedNode->GetTransform();
		transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * transform.localPos.x;
		transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * transform.localPos.y;

		transform.localPos.z += 10; // increase z-index little bit
		auto sprite = spriteTypes["explosion"][0];
		auto explos = spt<SpriteInst>(new SpriteInst(sprite, transform));
		this->explosions.push_back(explos);
		dynamicSprites->AddSprite(explos);
		dynamicSprites->RefreshZIndex();

	}
}

void GameView::OnStaticObjectChanged(spt<MapObjectChangedEvent> evt) {
	auto mapNode = evt->changedMapTile;
	// get sprite entity and change its frame
	spt<SpriteInst> sprEntity = staticSpriteMap[mapNode->GetPosition()];
	auto sprite = spriteTypes[mapNode->GetMapTileName()][0];
	sprEntity->sprite = sprite;
}

void GameView::OnRigTaken(spt<MapObjectChangedEvent> evt) {
	auto node = evt->changedNode;
	Vec2i pos = Vec2i(node->GetTransform().localPos.x, node->GetTransform().localPos.y);
	auto fact = node->GetAttr<Faction>(ATTR_FACTION);

	if (evt->changeType == ObjectChangeType::RIG_TAKEN && fact == playerModel->GetFaction()) this->CreateAnimationText("You have captured empty rig!");
	if (evt->changeType == ObjectChangeType::RIG_TAKEN && fact != playerModel->GetFaction()) this->CreateAnimationText("Enemy has captured empty rig!");
	if (evt->changeType == ObjectChangeType::RIG_CAPTURED && fact == playerModel->GetFaction()) this->CreateAnimationText("You have captured enemy rig!");
	if (evt->changeType == ObjectChangeType::RIG_CAPTURED && fact != playerModel->GetFaction()) this->CreateAnimationText("Enemy has captured your rig!");

	if (evt->changeType == ObjectChangeType::RIG_TAKEN) {
		rigsToAnimate.push_back(evt->changedNode);

		auto& movingObjects = gameModel->GetMovingObjects();

		// update transformation of all objects
		for (auto& dynObj : movingObjects) {
			int id = dynObj->GetId();
			auto sprite = dynamicSpriteEntities[id];
			auto faction = dynObj->GetAttr<Faction>(ATTR_FACTION);
			if (faction == Faction::RED) {
				int frameToSet = spriteTypes["worker_red"][0].GetFrame();
				sprite->sprite = Sprite(defaultSpriteSet, frameToSet);
			}
			else {
				int frameToSet = spriteTypes["worker_blue"][0].GetFrame();
				sprite->sprite = Sprite(defaultSpriteSet, frameToSet);
			}
		}


		if (fact == Faction::BLUE) {
			staticSpriteMap[Vec2i(pos.x, pos.y)]->sprite = spriteTypes["rig_blue_start"][0];
			staticSpriteMap[Vec2i(pos.x + 1, pos.y)]->sprite = spriteTypes["rig_blue_start"][1];
			staticSpriteMap[Vec2i(pos.x, pos.y + 1)]->sprite = spriteTypes["rig_blue_start"][2];
			staticSpriteMap[Vec2i(pos.x + 1, pos.y + 1)]->sprite = spriteTypes["rig_blue_start"][3];
		}
		else if (fact == Faction::RED) {
			staticSpriteMap[Vec2i(pos.x, pos.y)]->sprite = spriteTypes["rig_red_start"][0];
			staticSpriteMap[Vec2i(pos.x + 1, pos.y)]->sprite = spriteTypes["rig_red_start"][1];
			staticSpriteMap[Vec2i(pos.x, pos.y + 1)]->sprite = spriteTypes["rig_red_start"][2];
			staticSpriteMap[Vec2i(pos.x + 1, pos.y + 1)]->sprite = spriteTypes["rig_red_start"][3];
		}
	}
	else {
		if (fact == Faction::BLUE) {
			staticSpriteMap[Vec2i(pos.x, pos.y)]->sprite = spriteTypes["rig_blue"][0];
			staticSpriteMap[Vec2i(pos.x + 1, pos.y)]->sprite = spriteTypes["rig_blue"][1];
			staticSpriteMap[Vec2i(pos.x, pos.y + 1)]->sprite = spriteTypes["rig_blue"][2];
			staticSpriteMap[Vec2i(pos.x + 1, pos.y + 1)]->sprite = spriteTypes["rig_blue"][3];
		}
		else if (fact == Faction::RED) {
			staticSpriteMap[Vec2i(pos.x, pos.y)]->sprite = spriteTypes["rig_red"][0];
			staticSpriteMap[Vec2i(pos.x + 1, pos.y)]->sprite = spriteTypes["rig_red"][1];
			staticSpriteMap[Vec2i(pos.x, pos.y + 1)]->sprite = spriteTypes["rig_red"][2];
			staticSpriteMap[Vec2i(pos.x + 1, pos.y + 1)]->sprite = spriteTypes["rig_red"][3];
		}
	}
}

void GameView::UpdateMovingObjects() {
	auto& movingObjects = gameModel->GetMovingObjects();

	StrId stateIdle = StrId(STATE_WORKER_IDLE);
	StrId stateBuild = StrId(STATE_WORKER_BUILD);
	StrId stateAttract = StrId(STATE_WORKER_ATTRACTOR_FOLLOW);


	// update transformation of all objects
	for (auto& dynObj : movingObjects) {
		int id = dynObj->GetId();
		auto sprite = dynamicSpriteEntities[id];

		sprite->transform.localPos.x = defaultSpriteSet->GetSpriteWidth() * dynObj->GetTransform().localPos.x - defaultSpriteSet->GetSpriteWidth() / 2;
		sprite->transform.localPos.y = defaultSpriteSet->GetSpriteHeight() * dynObj->GetTransform().localPos.y - defaultSpriteSet->GetSpriteHeight() / 2;
		sprite->transform.rotation = dynObj->GetTransform().rotation;

		if (dynObj->HasState(stateIdle)) {
			// set image according to state
			sprite->sprite = spriteTypes[dynObj->GetTag()][0];
		}
		else if (dynObj->HasState(stateBuild) || dynObj->HasState(stateAttract)) {
			// set animation
			int startFrame = spriteTypes[dynObj->GetTag()][0].GetFrame() + 1;
			int actualFrame = sprite->sprite.GetFrame();
			int frames = 3;

			if (CogGetFrameCounter() % 2 == 0) {
				int frameToSet = 0;
				if (startFrame > actualFrame) frameToSet = startFrame;
				else if (actualFrame == (startFrame + frames - 1)) frameToSet = startFrame;
				else frameToSet = actualFrame + 1;
				sprite->sprite = GetSprite(frameToSet);
			}
		}
	}
}

void GameView::UpdateRigAnimations() {
	auto spriteType = spriteTypes["rig_blue"];
	vector<Node*> blueRigs;
	gameModel->GetRigsByFaction(Faction::BLUE, blueRigs);

	for (auto& rig : blueRigs) {
		if (!spriteType.empty()) {
			auto startFrame = spriteType[0].GetFrame();

			auto pos = ofVec2f(rig->GetTransform().localPos);

			auto sprite1 = this->staticSpriteMap[Vec2i(pos.x, pos.y)];
			auto sprite2 = this->staticSpriteMap[Vec2i(pos.x + 1, pos.y)];
			auto sprite3 = this->staticSpriteMap[Vec2i(pos.x, pos.y + 1)];
			auto sprite4 = this->staticSpriteMap[Vec2i(pos.x + 1, pos.y + 1)];

			if (sprite1 && sprite2 && sprite3 &&sprite4) {
				if (sprite1->sprite.GetFrame() >= startFrame) { // prevent from rig capturing animation
					auto actualFrame = sprite1->sprite.GetFrame() + 4;
					if (actualFrame == startFrame + 4 * 3) actualFrame = startFrame;

					sprite1->sprite = GetSprite(actualFrame);
					sprite2->sprite = GetSprite(actualFrame + 1);
					sprite3->sprite = GetSprite(actualFrame + 2);
					sprite4->sprite = GetSprite(actualFrame + 3);
				}
			}
		}
	}

	spriteType = spriteTypes["rig_red"];
	vector<Node*> redRigs;
	gameModel->GetRigsByFaction(Faction::RED, redRigs);

	for (auto& rig : redRigs) {
		if (!spriteType.empty()) {
			auto startFrame = spriteType[0].GetFrame();

			auto pos = ofVec2f(rig->GetTransform().localPos);

			auto sprite1 = this->staticSpriteMap[Vec2i(pos.x, pos.y)];
			auto sprite2 = this->staticSpriteMap[Vec2i(pos.x + 1, pos.y)];
			auto sprite3 = this->staticSpriteMap[Vec2i(pos.x, pos.y + 1)];
			auto sprite4 = this->staticSpriteMap[Vec2i(pos.x + 1, pos.y + 1)];

			if (sprite1 && sprite2 && sprite3 &&sprite4) {
				if (sprite1->sprite.GetFrame() >= startFrame) { // prevent from rig capturing animation
					auto actualFrame = sprite1->sprite.GetFrame() + 4;
					if (actualFrame == startFrame + 4 * 3) actualFrame = startFrame;

					sprite1->sprite = GetSprite(actualFrame);
					sprite2->sprite = GetSprite(actualFrame + 1);
					sprite3->sprite = GetSprite(actualFrame + 2);
					sprite4->sprite = GetSprite(actualFrame + 3);
				}
			}
		}
	}
}

void GameView::UpdateBridgeAnimations() {
	auto spriteType = spriteTypes["explosion"];
	for (auto it = explosions.begin(); it != explosions.end();) {
		int startFrame = spriteType[0].GetFrame();
		int actualFrame = (*it)->sprite.GetFrame() + 1;
		if ((actualFrame - startFrame) > 11) {
			dynamicSprites->RemoveSprite(*it);
			it = explosions.erase(it);
		}
		else {
			(*it)->sprite = GetSprite(actualFrame);
			++it;
		}
	}
}

void GameView::UpdatePlatformAnimations() {
	int platformDefFrame = spriteTypes["rig_platform"][0].GetFrame();
	int platformStompBlueFrame = spriteTypes["rig_platform_stomp_blue"][0].GetFrame();
	int platformStompRedFrame = spriteTypes["rig_platform_stomp_red"][0].GetFrame();
	int platformStompBothFrame = spriteTypes["rig_platform_stomp_both"][0].GetFrame();

	for (auto& rig : gameModel->GetRigs()) {

		for (auto platformPos : rig.second->platforms) {
			auto platform = this->staticSpriteMap[platformPos];
			if (!platform) return;

			auto& holding = rig.second->factionHoldings[platformPos];

			if (holding.blueNumber == 0 && holding.redNumber == 0) {

				if (platform->sprite.GetFrame() != platformDefFrame) {
					platform->sprite = GetSprite(platformDefFrame);
				}

			}
			else if (holding.blueNumber > holding.redNumber) {

				if (platform->sprite.GetFrame() != platformStompBlueFrame) {
					platform->sprite = GetSprite(platformStompBlueFrame);
				}

			}
			else if (holding.blueNumber == holding.redNumber) {

				if (platform->sprite.GetFrame() != platformStompBothFrame) {
					platform->sprite = GetSprite(platformStompBothFrame);
				}

			}
			else {

				if (platform->sprite.GetFrame() != platformStompRedFrame) {
					platform->sprite = GetSprite(platformStompRedFrame);
				}

			}
		}
	}
}

void GameView::UpdateActionAnimations() {
	for (auto it = rigsToAnimate.begin(); it != rigsToAnimate.end();) {
		auto rig = (*it);
		int redStartPos = spriteTypes["rig_red_start"][0].GetFrame();
		int blueStartPos = spriteTypes["rig_blue_start"][0].GetFrame();
		int redEndPos = spriteTypes["rig_red"][0].GetFrame();
		int blueEndPos = spriteTypes["rig_blue"][0].GetFrame();

		auto rigPos = Vec2i(rig->GetTransform().localPos.x, rig->GetTransform().localPos.y);
		auto rigPart1 = staticSpriteMap[Vec2i(rigPos.x, rigPos.y)];
		auto rigPart2 = staticSpriteMap[Vec2i(rigPos.x + 1, rigPos.y)];
		auto rigPart3 = staticSpriteMap[Vec2i(rigPos.x, rigPos.y + 1)];
		auto rigPart4 = staticSpriteMap[Vec2i(rigPos.x + 1, rigPos.y + 1)];

		auto faction = rig->GetAttr<Faction>(ATTR_FACTION);
		int endPos = faction == Faction::RED ? redEndPos : blueEndPos;

		int actualIndex = rigPart1->sprite.GetFrame();
		if (actualIndex != endPos) {
			actualIndex += 4;
			rigPart1->sprite = GetSprite(actualIndex);
			rigPart2->sprite = GetSprite(actualIndex + 1);
			rigPart3->sprite = GetSprite(actualIndex + 2);
			rigPart4->sprite = GetSprite(actualIndex + 3);
			++it;
		}
		else {
			it = rigsToAnimate.erase(it);
		}
	}
}

void GameView::OnMultiplayerConnectionLost() {
	// display dialog that game waits for reconnect
	auto stage = GETCOMPONENT(Stage);
	auto scene = stage->FindSceneByName("connection_lost_dialog");
	stage->SwitchToScene(scene, TweenDirection::NONE);
}

void GameView::OnMultiplayerReconnect() {
	auto stage = GETCOMPONENT(Stage);
	stage->SwitchBackToScene(TweenDirection::NONE);
}