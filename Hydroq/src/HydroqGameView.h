#pragma once

#include "Behavior.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"
#include "HydroqGameModel.h"

/**
* Hydroq game view
*/
class HydroqGameView : public Behavior {

private:
	// link to default sprite set
	spt<SpriteSet> defaultSpriteSet;
	// collection of all sprite types, mapped by its name
	map<string, vector<Sprite>> spriteTypes;

	// collection of static sprites
	spt<SpritesShape> staticSprites;

	// collection of static sprites, mapping by position
	map<Vec2i, spt<SpriteEntity>> staticSpriteMap;
	// collection of dynamic sprites
	spt<SpritesShape> dynamicSprites;

	vector<spt<SpriteEntity>> explosions;
	// collection of dynamic sprites, mapping by identifier
	map<int, spt<SpriteEntity>> dynamicSpriteEntities;
	
	vector<Node*> rigsToAnimate;

	map<int, Sprite> spriteBuffer;

	HydroqGameModel* gameModel;
	bool firstUpdate = false;
	// the time the game has ended
	uint64 gameEndedTime = 0;
public:

	~HydroqGameView() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

	void LoadSprites(Setting sprites);

	/**
	Gets collection of static sprites
	*/
	spt<SpritesShape> GetStaticSprites() {
		return staticSprites;
	}

	/**
	* Gets collection of dynamic sprites (including both moving and dynamic objects)
	*/
	spt<SpritesShape> GetDynamicSprites() {
		return dynamicSprites;
	}

	void SaveMapImageToFile(string file);

	Sprite& GetSprite(int frame);

	virtual void Update(const uint64 delta, const uint64 absolute);

	Node* animNode = nullptr;
	Node* animTextNode = nullptr;
	void CreateAnimationText(string message);
};