//
//  ScreenInputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "ScreenInputManager.h"

#include "ScreenEvent.h"

#define MAX_DRAGGED_EVENTS_PER_FRAME 3
#define POOL_SIZE 256

ScreenInputManager* ScreenInputManager::getInstance()
{
    static ScreenInputManager instance = ScreenInputManager();
    return &instance;
}

void ScreenInputManager::onTouch(ScreenEventType type, float x, float y)
{
    if (type == ScreenEventType_DRAGGED
        && m_pool->getBufferSize() >= MAX_DRAGGED_EVENTS_PER_FRAME)
    {
        return;
    }
    
    addEvent(type, x, y);
}

void ScreenInputManager::process()
{
    m_pool->processBuffer();
}

std::vector<ScreenEvent*>& ScreenInputManager::getEvents()
{
    return m_pool->getObjects();
}

#pragma mark private

void ScreenInputManager::addEvent(ScreenEventType type, float x, float y)
{
    ScreenEvent* e = m_pool->newObject();
    e->setType(type);
    e->setX(x);
    e->setY(y);
    
    m_pool->add(e);
}

ScreenInputManager::ScreenInputManager() : m_pool(new NGRollingPool<ScreenEvent>(POOL_SIZE))
{
    // Empty
}
