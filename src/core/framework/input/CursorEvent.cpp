//
//  CursorEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/input/CursorEvent.h"

CursorEvent::CursorEvent(CursorEventType type, float x, float y)
{
    _type = type;
    _x = x;
    _y = y;
}

CursorEventType CursorEvent::getType()
{
	return _type;
}

void CursorEvent::setType(CursorEventType type)
{
	_type = type;
}

float CursorEvent::getX()
{
	return _x;
}

void CursorEvent::setX(float x)
{
	_x = x;
}

float CursorEvent::getY()
{
	return _y;
}

void CursorEvent::setY(float y)
{
	_y = y;
}
