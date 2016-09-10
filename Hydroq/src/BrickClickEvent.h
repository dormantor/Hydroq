#pragma once

#include "ofxCogMain.h"
#include "Events.h"


class BrickClickEvent : public MsgEvent {
public:
	
	int brickPosX;
	int brickPosY;
	pair<spt<Sprite>, Trans> brick;

	BrickClickEvent(int brickPosX, int brickPosY, pair<spt<Sprite>, Trans> brick) :
	brickPosX(brickPosX), brickPosY(brickPosY), brick(brick)
	{

	}
};