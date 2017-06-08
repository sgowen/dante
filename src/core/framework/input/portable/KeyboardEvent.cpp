//
//  KeyboardEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent(KeyboardEventType type, int status) :
m_type(type),
m_iStatus(status)
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

void KeyboardEvent::setStatus(int status)
{
    m_iStatus = status;
}

int KeyboardEvent::getStatus()
{
    return m_iStatus;
}

bool KeyboardEvent::isDown()
{
    return m_iStatus == KEYBOARD_STATUS_DOWN || m_iStatus == KEYBOARD_STATUS_HELD;
}

bool KeyboardEvent::isHeld()
{
    return m_iStatus == KEYBOARD_STATUS_HELD;
}

bool KeyboardEvent::isUp()
{
    return m_iStatus == KEYBOARD_STATUS_UP;
}
