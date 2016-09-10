#pragma once

#include "Behavior.h"
#include "HydroqDef.h"
#include "MsgPayloads.h"
#include "GameMap.h"
#include "GameEntity.h"
#include "GameModel.h"

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

	vector<spt<SpriteInst>> explosions;
	// collection of dynamic sprites, mapping by identifier
	map<int, spt<SpriteInst>> dynamicSpriteEntities;
	
	vector<Node*> rigsToAnimate;

	map<int, Sprite> spriteBuffer;

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

	void SaveMapImageToFile(string file);

	Sprite& GetSprite(int frame);

	virtual void Update(const uint64 delta, const uint64 absolute);

	Node* animNode = nullptr;
	Node* animTextNode = nullptr;
	void CreateAnimationText(string message);
};