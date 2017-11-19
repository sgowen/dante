//
//  GamePadEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/input/GamePadEvent.h"

GamePadEvent::GamePadEvent(GamePadEventType type, int index, float x, float y) :
_type(type),
_index(index),
_x(x),
_y(y)
{
	// Empty
}

GamePadEventType GamePadEvent::getType()
{
    return _type;
}

void GamePadEvent::setType(GamePadEventType touchType)
{
    _type = touchType;
}

int GamePadEvent::getIndex()
{
    return _index;
}

void GamePadEvent::setIndex(int index)
{
    _index = index;
}

float GamePadEvent::getX()
{
	return _x;
}

void GamePadEvent::setX(float x)
{
	_x = x;
}

float GamePadEvent::getY()
{
	return _y;
}

void GamePadEvent::setY(float y)
{
	_y = y;
}

bool GamePadEvent::isPressed()
{
    return _x > 0;
}
