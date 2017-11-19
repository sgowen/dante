//
//  KeyboardEvent.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent(unsigned short key, int status) : _sKey(key), _status(status)
{
	// Empty
}

void KeyboardEvent::setKey(unsigned short key)
{
    _sKey = key;
}

unsigned short KeyboardEvent::getKey()
{
    return _sKey;
}

void KeyboardEvent::setStatus(int status)
{
    _status = status;
}

int KeyboardEvent::getStatus()
{
    return _status;
}

bool KeyboardEvent::isDown()
{
    return _status == KEYBOARD_STATUS_DOWN || _status == KEYBOARD_STATUS_HELD;
}

bool KeyboardEvent::isHeld()
{
    return _status == KEYBOARD_STATUS_HELD;
}

bool KeyboardEvent::isUp()
{
    return _status == KEYBOARD_STATUS_UP;
}
