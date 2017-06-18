//
//  KeyboardEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent(unsigned short key, int status) : m_sKey(key), m_iStatus(status)
{
	// Empty
}

void KeyboardEvent::setKey(unsigned short key)
{
    m_sKey = key;
}

unsigned short KeyboardEvent::getKey()
{
    return m_sKey;
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
