//
//  CursorEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "CursorEvent.h"

CursorEvent::CursorEvent(CursorEventType type, float x, float y)
{
    m_type = type;
    m_fX = x;
    m_fY = y;
}

CursorEventType CursorEvent::getType()
{
	return m_type;
}

void CursorEvent::setType(CursorEventType type)
{
	m_type = type;
}

float CursorEvent::getX()
{
	return m_fX;
}

void CursorEvent::setX(float x)
{
	m_fX = x;
}

float CursorEvent::getY()
{
	return m_fY;
}

void CursorEvent::setY(float y)
{
	m_fY = y;
}
