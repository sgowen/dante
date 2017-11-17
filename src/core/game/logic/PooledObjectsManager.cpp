//
//  PooledObjectsManager.cpp
//  dante
//
//  Created by Stephen Gowen on 6/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "PooledObjectsManager.h"

#include "InputState.h"
#include "MathUtil.h"
#include "FrameworkConstants.h"

#define POOL_SIZE MAX(NW_SERVER_TIMEOUT, NW_CLIENT_TIMEOUT) * (MAX_NUM_PLAYERS_PER_SERVER + 1) * 60

PooledObjectsManager* PooledObjectsManager::getInstance()
{
    static PooledObjectsManager instance = PooledObjectsManager();
    return &instance;
}

IInputState* PooledObjectsManager::borrowInputState()
{
    IInputState* ret = nullptr;
    while ((ret = POOLED_OBJ_MGR->_pool->newObject())->isInUse())
    {
        // Continue
    }
    
    ret->reset();
    
    ret->setInUse(true);
    
    return ret;
}

#pragma mark private

PooledObjectsManager::PooledObjectsManager() : _pool(new NGRollingPool<InputState>(POOL_SIZE))
{
    // Empty
}

PooledObjectsManager::~PooledObjectsManager()
{
    delete _pool;
}
