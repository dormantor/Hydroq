#pragma once

#include "Component.h"
#include "HydroqDef.h"
#include "Events.h"
#include "HydMap.h"
#include "HydEntity.h"
#include "HydroqGameModel.h"

/**
* Hydroq game view
*/
class HydroqGameView : public Component {

	OBJECT(HydroqGameView)

private:
	// link to default sprite set
	spt<SpriteSet> defaultSpriteSet;
	// collection of all sprite types, mapped by its name
	map<string, spt<Sprite>> spriteTypes;

	// collection of static sprites
	spt<SpritesShape> staticSprites;
	// collection of static sprites, mapping by position
	map<Vec2i, spt<SpriteEntity>> staticSpriteMap;
	// collection of dynamic sprites
	spt<SpritesShape> dynamicSprites;
	// collection of dynamic sprites, mapping by identifier
	map<int, spt<SpriteEntity>> dynamicSpriteEntities;
	
public:

	~HydroqGameView() {

	}

	void Init();

	void Init(spt<ofxXml> xml) {
		Init();
	}

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

	virtual void Update(const uint64 delta, const uint64 absolute);
};