//
//  KeyboardInputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "KeyboardInputManager.h"

#include "KeyboardEvent.h"
#include "KeyboardLookup.h"

#define POOL_SIZE 256

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
    m_pool->processBuffer();
}

std::vector<KeyboardEvent*>& KeyboardInputManager::getEvents()
{
    return m_pool->getObjects();
}

#pragma mark private

void KeyboardInputManager::addEvent(unsigned short key, bool isUp)
{
    bool wasLastEventDown = false;
    
    auto q = m_lastKnownKeyStates.find(key);
    
    if (q != m_lastKnownKeyStates.end())
    {
        bool wasUp = q->second;
        wasLastEventDown = !wasUp;
    }
    
    m_lastKnownKeyStates[key] = isUp;
    
    KeyboardEvent* e = m_pool->newObject();
    e->setKey(key);
    e->setStatus(isUp ? KEYBOARD_STATUS_UP : wasLastEventDown ? KEYBOARD_STATUS_HELD : KEYBOARD_STATUS_DOWN);
    
    m_pool->add(e);
}

KeyboardInputManager::KeyboardInputManager() : m_pool(new NGRollingPool<KeyboardEvent>(POOL_SIZE))
{
    // Empty
}

KeyboardInputManager::~KeyboardInputManager()
{
    delete m_pool;
}
