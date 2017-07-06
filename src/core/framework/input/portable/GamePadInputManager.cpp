//
//  GamePadInputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "GamePadInputManager.h"

#include "GamePadEvent.h"

#include "FrameworkConstants.h"
#include "Timing.h"

#define POOL_SIZE 8192

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

bool GamePadInputManager::isControllerConnected()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    return (time - m_fTimeSinceLastGamePadEvent) < 5;
}

#pragma mark private

void GamePadInputManager::addEvent(GamePadEventType type, int index, float x, float y)
{
    if (index < 0 || index >= MAX_NUM_PLAYERS_PER_SERVER)
    {
        return;
    }
    
    GamePadEvent* e = m_pool->newObject();
    e->setType(type);
    e->setIndex(index);
    e->setX(x);
    e->setY(y);
    
    m_pool->add(e);
    
    m_fTimeSinceLastGamePadEvent = Timing::getInstance()->getFrameStartTime();
}

GamePadInputManager::GamePadInputManager() : m_pool(new NGRollingPool<GamePadEvent>(POOL_SIZE)), m_fTimeSinceLastGamePadEvent(0)
{
    // Empty
}

GamePadInputManager::~GamePadInputManager()
{
    delete m_pool;
}
