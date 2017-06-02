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

#define POOL_SIZE 256

PooledObjectsManager* PooledObjectsManager::getInstance()
{
    static PooledObjectsManager instance = PooledObjectsManager();
    return &instance;
}

IInputState* PooledObjectsManager::borrowBaseType()
{
    return POOLED_OBJ_MGR->m_pool->newObject();
}

InputState* PooledObjectsManager::borrow()
{
    return POOLED_OBJ_MGR->m_pool->newObject();
}

#pragma mark private

PooledObjectsManager::PooledObjectsManager() : m_pool(new NGRollingPool<InputState>(POOL_SIZE))
{
    // Empty
}

PooledObjectsManager::~PooledObjectsManager()
{
    delete m_pool;
}
