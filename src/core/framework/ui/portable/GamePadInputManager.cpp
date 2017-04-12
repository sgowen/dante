//
//  GamePadInputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "GamePadInputManager.h"

#include "GamePadEvent.h"

#define POOL_SIZE 1024

GamePadInputManager* GamePadInputManager::getInstance()
{
    static GamePadInputManager instance = GamePadInputManager();
    return &instance;
}

void GamePadInputManager::onInput(GamePadEventType type, int index, float x, float y)
{
    addEvent(type, index, x, y);
}

void GamePadInputManager::process()
{
    m_pool->processBuffer();
}

std::vector<GamePadEvent*>& GamePadInputManager::getEvents()
{
    return m_pool->getObjects();
}

#pragma mark private

void GamePadInputManager::addEvent(GamePadEventType type, int index, float x, float y)
{
    GamePadEvent* e = m_pool->newObject();
    e->setType(type);
    e->setIndex(index);
    e->setX(x);
    e->setY(y);
    
    m_pool->add(e);
}

GamePadInputManager::GamePadInputManager() : m_pool(new NGRollingPool<GamePadEvent>(POOL_SIZE))
{
    // Empty
}
