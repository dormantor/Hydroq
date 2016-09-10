#pragma once

#include "ofxCogMain.h"
#include "BrickClickEvent.h"
#include "HydroqDef.h"

class HydroqBoard : public Behavior {
	OBJECT_PROTOTYPE(HydroqBoard)


private:
	BrickMap bricks;
	vector<spt<Sprite>> allSprites;
	Grid starGraph;
	AStarSearch starSearch;
	ResourceCache* cache;
	Settings mapConfig;

	ofVec2f hitPos;

	int actualState = 0;
	Vec2i start;
	Vec2i goal;
	vector<Vec2i> path;
public:

	void LoadBricks() {
		// load map config
		auto xml = CogLoadXMLFile("mapconfig.xml");
		xml->pushTag("settings");
		mapConfig.LoadFromXml(xml);
		xml->popTag();

		// load map
		MapLoader mapLoad = MapLoader();
		this->bricks = mapLoad.LoadFromPNGImage("map_1.png", mapConfig.GetSetting("names"));
	}

	void Init() {
		cache = GETCOMPONENT(ResourceCache);

		LoadBricks();

		// create collection of all sprites
		auto crates = vector<pair<spt<Sprite>, Trans>>();

		TransformMath mth = TransformMath();
		allSprites = vector<spt<Sprite>>();

		auto spriteSheet = cache->GetSpriteSheet("game_board");
		auto spriteSet = spriteSheet->GetDefaultSpriteSet();

		// push all sprites
		for (int i = 0; i < 8; i++) {
			allSprites.push_back(spt<Sprite>(new Sprite(spriteSet, i)));
		}

		// initialize A* algorithm
		starGraph = Grid(bricks.width, bricks.height);


		for (auto& brick : bricks.bricks) {

			int spriteIndex = mapConfig.GetSettingValInt("sprites", brick.name);

			if (spriteIndex != 1) {
				// wall
				starGraph.AddBlocks(brick.posX, brick.posY, brick.posX, brick.posY);
			}

			spt<Sprite> crate = allSprites[spriteIndex];
			Trans transform = Trans();

			transform.localPos.x = spriteSet->GetSpriteWidth() * brick.posX;
			transform.localPos.y = spriteSet->GetSpriteHeight() * brick.posY;

			crates.push_back(std::make_pair(crate, transform));
		}

		spt<SpritesShape> shape = spt<SpritesShape>(new SpritesShape("map_board", crates));
		owner->SetShape(shape);

		auto parentShape = owner->GetParent()->GetShape<spt<Cog::Rectangle>>();
		parentShape->SetWidth(shape->GetWidth());
		parentShape->SetHeight(shape->GetHeight());

		RegisterListening(owner->GetScene(), ACT_OBJECT_HIT_STARTED, ACT_OBJECT_HIT_ENDED);
	}

	void OnMessage(Msg& msg) {
		if (msg.GetSourceObject()->GetId() == owner->GetId() || msg.GetSourceObject()->GetId() == owner->GetParent()->GetId()) {


			InputEvent* touch = static_cast<InputEvent*>(msg.GetData());

			if (msg.GetAction() == ACT_OBJECT_HIT_STARTED) {
				hitPos = touch->input->position;
			}
			else if (msg.GetAction() == ACT_OBJECT_HIT_ENDED) {
				ofVec2f endPos = touch->input->position;

				if (endPos.distance(hitPos) < 10) {
					auto shape = owner->GetShape();
					auto crates = static_cast<spt<SpritesShape>> (shape)->GetSprites();

					// get pressed brick
					float shapeWidth = shape->GetWidth()*owner->GetTransform().absScale.x;
					float shapeHeight = shape->GetHeight()*owner->GetTransform().absScale.y;
					float absPosX = owner->GetTransform().absPos.x;
					float absPosY = owner->GetTransform().absPos.y;

					float distX = (endPos.x - absPosX);
					float distY = (endPos.y - absPosY);

					int brickPosX = (int)((distX / shapeWidth)*bricks.width);
					int brickPosY = (int)((distY / shapeHeight)*bricks.height);

					auto crate = crates[brickPosY*bricks.width + brickPosX];

					SendMessageNoBubbling(ACT_BRICK_CLICKED, 0, new BrickClickEvent(brickPosX, brickPosY, crate), owner);
					/*
					// get index of pressed brick
					auto& crate = crates[brickPosY*bricks.width + brickPosX];
					int actualIndex = crate.first->GetFrame();

					if (actualIndex != 1) return;

					if (actualState == 2) {
						// restart
						for (Vec2i loc : path) {
							crates[loc.y*bricks.width + loc.x].first = (allSprites[1]);
						}
					}

					if (actualState == 0 || actualState == 2) {
						// neither target nor source is specified
						actualState = 1;
						crate.first = (allSprites[6]);
						start = Vec2i{ brickPosX, brickPosY };
					}
					else if (actualState == 1) {
						// source specified
						actualState = 2;
						goal = Vec2i{ brickPosX, brickPosY };
						crate.first = (allSprites[6]);

						// path
						unordered_map<Vec2i, Vec2i> came_from;
						// cost
						unordered_map<Vec2i, int> cost_so_far;

						// search path
						this->starSearch.Search(starGraph, start, goal, came_from, cost_so_far);
						path = this->starSearch.CalcPathFromJumps(start, goal, came_from);

						for (Vec2i loc : path) {

							if (loc != goal && loc != start) {
								crates[loc.y*bricks.width + loc.x].first = (allSprites[7]);
							}
						}
					}*/
				}
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};