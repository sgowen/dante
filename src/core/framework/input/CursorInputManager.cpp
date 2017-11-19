//
//  CursorInputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/input/CursorInputManager.h"

#include "framework/input/CursorEvent.h"

#define MAX_DRAGGED_EVENTS_PER_FRAME 3
#define POOL_SIZE 256

CursorInputManager* CursorInputManager::getInstance()
{
    static CursorInputManager instance = CursorInputManager();
    return &instance;
}

void CursorInputManager::onTouch(CursorEventType type, float x, float y)
{
    if (type == CursorEventType_DRAGGED
        && _pool->getBufferSize() >= MAX_DRAGGED_EVENTS_PER_FRAME)
    {
        return;
    }
    
    addEvent(type, x, y);
}

void CursorInputManager::process()
{
    _pool->processBuffer();
}

std::vector<CursorEvent*>& CursorInputManager::getEvents()
{
    return _pool->getObjects();
}

#pragma mark private

void CursorInputManager::addEvent(CursorEventType type, float x, float y)
{
    CursorEvent* e = _pool->newObject();
    e->setType(type);
    e->setX(x);
    e->setY(y);
    
    _pool->add(e);
}

CursorInputManager::CursorInputManager() : _pool(new NGRollingPool<CursorEvent>(POOL_SIZE))
{
    // Empty
}

CursorInputManager::~CursorInputManager()
{
    delete _pool;
}
