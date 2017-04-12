//
//  ScreenEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "ScreenEvent.h"

ScreenEvent::ScreenEvent(ScreenEventType type, float x, float y)
{
    m_type = type;
    m_fX = x;
    m_fY = y;
}

ScreenEventType ScreenEvent::getType()
{
	return m_type;
}

void ScreenEvent::setType(ScreenEventType type)
{
	m_type = type;
}

float ScreenEvent::getX()
{
	return m_fX;
}

void ScreenEvent::setX(float x)
{
	m_fX = x;
}

float ScreenEvent::getY()
{
	return m_fY;
}

void ScreenEvent::setY(float y)
{
	m_fY = y;
}
