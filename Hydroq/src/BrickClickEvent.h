#pragma once

#include "ofxCogMain.h"
#include "Events.h"


class BrickClickEvent : public MsgEvent {
public:
	
	int brickPosX;
	int brickPosY;
	spt<SpriteEntity> brick;

	BrickClickEvent(int brickPosX, int brickPosY, spt<SpriteEntity> brick) :
	brickPosX(brickPosX), brickPosY(brickPosY), brick(brick)
	{

	}
};