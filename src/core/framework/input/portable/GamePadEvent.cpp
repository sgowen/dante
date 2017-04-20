//
//  GamePadEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "GamePadEvent.h"

GamePadEvent::GamePadEvent(GamePadEventType type, int index, float x, float y) :
m_type(type),
m_iIndex(index),
m_fX(x),
m_fY(y)
{
	// Empty
}

GamePadEventType GamePadEvent::getType()
{
    return m_type;
}

void GamePadEvent::setType(GamePadEventType touchType)
{
    m_type = touchType;
}

int GamePadEvent::getIndex()
{
    return m_iIndex;
}

void GamePadEvent::setIndex(int index)
{
    m_iIndex = index;
}

float GamePadEvent::getX()
{
	return m_fX;
}

void GamePadEvent::setX(float x)
{
	m_fX = x;
}

float GamePadEvent::getY()
{
	return m_fY;
}

void GamePadEvent::setY(float y)
{
	m_fY = y;
}

bool GamePadEvent::isButtonPressed()
{
    return m_fX > 0;
}
