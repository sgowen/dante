//
//  PooledObjectsManager.cpp
//  dante
//
//  Created by Stephen Gowen on 6/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/PooledObjectsManager.h"

#include "game/network/MainInputState.h"
#include "framework/math/MathUtil.h"
#include "framework/util/FrameworkConstants.h"

#define POOL_SIZE MAX(NW_SERVER_TIMEOUT, NW_CLIENT_TIMEOUT) * (MAX_NUM_PLAYERS_PER_SERVER + 1) * 60

PooledObjectsManager* PooledObjectsManager::getInstance()
{
    static PooledObjectsManager instance = PooledObjectsManager();
    return &instance;
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
