//
//  KeyboardInputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/input/KeyboardInputManager.h>

#include <framework/input/KeyboardEvent.h>
#include <framework/input/KeyboardLookup.h>

#define POOL_SIZE 4096

KeyboardInputManager* KeyboardInputManager::getInstance()
{
    static KeyboardInputManager instance = KeyboardInputManager();
    return &instance;
}

void KeyboardInputManager::onInput(unsigned short key, bool isUp)
{
    addEvent(key, isUp);
}

void KeyboardInputManager::process()
{
    _pool->processBuffer();
}

std::vector<KeyboardEvent*>& KeyboardInputManager::getEvents()
{
    return _pool->getObjects();
}

#pragma mark private

void KeyboardInputManager::addEvent(unsigned short key, bool isUp)
{
    bool wasLastEventDown = false;
    
    auto q = _lastKnownKeyStates.find(key);
    
    if (q != _lastKnownKeyStates.end())
    {
        bool wasUp = q->second;
        wasLastEventDown = !wasUp;
    }
    
    _lastKnownKeyStates[key] = isUp;
    
    KeyboardEvent* e = _pool->newObject();
    e->setKey(key);
    e->setStatus(isUp ? KEYBOARD_STATUS_UP : wasLastEventDown ? KEYBOARD_STATUS_HELD : KEYBOARD_STATUS_DOWN);
    
    _pool->add(e);
}

KeyboardInputManager::KeyboardInputManager() : _pool(new NGRollingPool<KeyboardEvent>(POOL_SIZE))
{
    // Empty
}

KeyboardInputManager::~KeyboardInputManager()
{
    delete _pool;
}
