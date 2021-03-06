#pragma once

#include "Behavior.h"
#include "HydroqDef.h"
#include "MsgPayloads.h"
#include "GameMap.h"
#include "GameModel.h"
#include "PlayerModel.h"

/**
* Hydroq game view
*/
class GameView : public Behavior {

private:
	// link to default sprite set
	spt<SpriteSet> defaultSpriteSet;
	// collection of all sprite types, mapped by its name
	map<string, vector<Sprite>> spriteTypes;

	// collection of static sprites
	spt<MultiSpriteMesh> staticSprites;

	// collection of static sprites, mapping by position
	map<Vec2i, spt<SpriteInst>> staticSpriteMap;
	// collection of dynamic sprites
	spt<MultiSpriteMesh> dynamicSprites;
	// hydroq settings
	Settings hydroqSettings;

	vector<spt<SpriteInst>> explosions;
	// collection of dynamic sprites, mapping by identifier
	map<int, spt<SpriteInst>> dynamicSpriteEntities;
	
	vector<Node*> rigsToAnimate;

	map<int, Sprite> spriteBuffer;
	
	PlayerModel* playerModel;
	GameModel* gameModel;
	bool firstUpdate = false;
	// the time the game has ended
	uint64 gameEndedTime = 0;
public:

	~GameView() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

	void LoadSprites(Setting sprites);

	/**
	Gets collection of static sprites
	*/
	spt<MultiSpriteMesh> GetStaticSprites() {
		return staticSprites;
	}

	/**
	* Gets collection of dynamic sprites (including both moving and dynamic objects)
	*/
	spt<MultiSpriteMesh> GetDynamicSprites() {
		return dynamicSprites;
	}

	/**
	* Saves map image into file (for debug purposes)
	*/
	void SaveMapImageToFile(string file);

	/**
	* Gets sprite by its frame
	*/
	Sprite& GetSprite(int frame);

	virtual void Update(const uint64 delta, const uint64 absolute);

	Node* animNode = nullptr;
	Node* animTextNode = nullptr;
	void CreateAnimationText(string message);

	void OnMultiplayerConnectionLost();
	void OnMultiplayerReconnect();
protected:
	// =================== OnMessage actions ======================
	void OnDynamicObjectCreated(spt<MapObjectChangedEvent> evt);
	void OnDynamicObjectRemoved(spt<MapObjectChangedEvent> evt);
	void OnStaticObjectChanged(spt<MapObjectChangedEvent> evt);
	void OnRigTaken(spt<MapObjectChangedEvent> evt);
	// =================== Update actions =========================
	void UpdateMovingObjects();
	void UpdateRigAnimations();
	void UpdateBridgeAnimations();
	void UpdatePlatformAnimations();
	void UpdateActionAnimations();
};