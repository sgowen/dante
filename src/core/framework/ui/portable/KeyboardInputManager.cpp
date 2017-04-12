//
//  KeyboardInputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "KeyboardInputManager.h"

#include "KeyboardEvent.h"

#define POOL_SIZE 256

KeyboardInputManager* KeyboardInputManager::getInstance()
{
    static KeyboardInputManager instance = KeyboardInputManager();
    return &instance;
}

void KeyboardInputManager::onInput(KeyboardEventType type, bool isUp)
{
    addEvent(type, isUp);
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

void KeyboardInputManager::addEvent(KeyboardEventType type, bool isUp)
{
    KeyboardEvent* e = m_pool->newObject();
    e->setType(type);
    e->setUp(isUp);
    
    m_pool->add(e);
}

KeyboardInputManager::KeyboardInputManager() : m_pool(new NGRollingPool<KeyboardEvent>(POOL_SIZE))
{
    // Empty
}
