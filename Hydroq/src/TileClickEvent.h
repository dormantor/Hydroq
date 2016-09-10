#pragma once

#include "ofxCogMain.h"
#include "Events.h"


class TileClickEvent : public MsgEvent {
public:
	
	int brickPosX;
	int brickPosY;
	spt<SpriteInst> brick;

	TileClickEvent(int brickPosX, int brickPosY, spt<SpriteInst> brick) :
	brickPosX(brickPosX), brickPosY(brickPosY), brick(brick)
	{

	}
};