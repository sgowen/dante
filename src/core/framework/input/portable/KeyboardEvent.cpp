//
//  KeyboardEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent(KeyboardEventType type, bool isUp) :
m_type(type),
m_isUp(isUp)
{
	// Empty
}

KeyboardEventType KeyboardEvent::getType()
{
    return m_type;
}

void KeyboardEvent::setType(KeyboardEventType touchType)
{
    m_type = touchType;
}

bool KeyboardEvent::isUp()
{
    return m_isUp;
}

void KeyboardEvent::setUp(bool isUp)
{
    m_isUp = isUp;
}
