//
//  PooledObjectsManager.cpp
//  dante
//
//  Created by Stephen Gowen on 6/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/PooledObjectsManager.h>

#include <game/game/MainInputState.h>
#include <framework/util/MathUtil.h>
#include <framework/util/Constants.h>

#define POOL_SIZE MAX(NW_SERVER_TIMEOUT, NW_CLIENT_TIMEOUT) * (MAX_NUM_PLAYERS_PER_SERVER * 2) * 60

PooledObjectsManager* PooledObjectsManager::s_instance = NULL;

void PooledObjectsManager::create()
{
    assert(!s_instance);
    
    s_instance = new PooledObjectsManager();
}

PooledObjectsManager * PooledObjectsManager::getInstance()
{
    return s_instance;
}

void PooledObjectsManager::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

InputState* PooledObjectsManager::borrowInputState()
{
    InputState* ret = NULL;
    while ((ret = POOLED_OBJ_MGR->_pool->newObject())->isInUse())
    {
        // Continue
    }
    
    ret->reset();
    
    ret->setInUse(true);
    
    return ret;
}

#pragma mark private

PooledObjectsManager::PooledObjectsManager() : _pool(new NGRollingPool<MainInputState>(POOL_SIZE))
{
    // Empty
}

PooledObjectsManager::~PooledObjectsManager()
{
    delete _pool;
}
