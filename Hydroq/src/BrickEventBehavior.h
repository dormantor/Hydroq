#pragma once

#include "ofxCogMain.h"
#include "HydroqDef.h"

class BrickEventBehavior : public Behavior {
	OBJECT_PROTOTYPE(BrickEventBehavior)


	Node* objectBoard;
	ResourceCache* cache;
	spt<SpriteSheet> spriteSheet;
	spt<SpriteSet> spriteSet;

	void Init() {
		RegisterListening(owner->GetScene(), ACT_BRICK_CLICKED);

		objectBoard = owner->GetScene()->FindNodeByTag("object_board");

		auto crates = vector<pair<spt<Sprite>, Trans>>();
		spt<SpritesShape> shape = spt<SpritesShape>(new SpritesShape("object_board", crates));
		objectBoard->SetShape(shape);


		cache = GETCOMPONENT(ResourceCache);

		spriteSheet = cache->GetSpriteSheet("game_board");
		spriteSet = spriteSheet->GetDefaultSpriteSet();
	}


	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_BRICK_CLICKED) {
			int targetId = msg.GetSourceObject()->GetId();

			auto eventD = msg.GetDataS<BrickClickEvent>();

			spt<Sprite> crate = spt<Sprite>(new Sprite(spriteSet, 14));
			Trans transform = Trans();

			transform.localPos.x = spriteSet->GetSpriteWidth() * eventD->brickPosX;
			transform.localPos.y = spriteSet->GetSpriteHeight() * eventD->brickPosY;

			spt<SpritesShape> shapes = objectBoard->GetShape<spt<SpritesShape>>();
			shapes->GetSprites().push_back(std::make_pair(crate, transform));
		}
	}



public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};