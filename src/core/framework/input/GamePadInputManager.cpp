//
//  GamePadInputManager.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 2/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/input/GamePadInputManager.h>

#include <framework/input/GamePadEvent.h>

#include <framework/util/Constants.h>

#define POOL_SIZE 16384

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
    _pool->processBuffer();
}

std::vector<GamePadEvent*>& GamePadInputManager::getEvents()
{
    return _pool->getObjects();
}

#pragma mark private

void GamePadInputManager::addEvent(GamePadEventType type, int index, float x, float y)
{
    if (index < 0 || index >= MAX_NUM_PLAYERS_PER_SERVER)
    {
        return;
    }
    
    GamePadEvent* e = _pool->newObject();
    e->setType(type);
    e->setIndex(index);
    e->setX(x);
    e->setY(y);
    
    _pool->add(e);
}

GamePadInputManager::GamePadInputManager() : _pool(new NGRollingPool<GamePadEvent>(POOL_SIZE)), _timeSinceLastGamePadEvent(-5)
{
    // Empty
}

GamePadInputManager::~GamePadInputManager()
{
    delete _pool;
}
