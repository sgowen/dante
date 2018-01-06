//
//  PooledObjectsManager.h
//  dante
//
//  Created by Stephen Gowen on 6/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__PooledObjectsManager__
#define __noctisgames__PooledObjectsManager__

#include "framework/util/NGRollingPool.h"

#include <vector>

#define POOLED_OBJ_MGR (PooledObjectsManager::getInstance())

class InputState;
class MainInputState;

class PooledObjectsManager
{
public:
    static void create();
    static PooledObjectsManager* getInstance();
    static void destroy();
    
    static InputState* borrowInputState();
    
private:
    static PooledObjectsManager* s_instance;
    
    NGRollingPool<MainInputState>* _pool;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    PooledObjectsManager();
    ~PooledObjectsManager();
    PooledObjectsManager(const PooledObjectsManager&);
    PooledObjectsManager& operator=(const PooledObjectsManager&);
};

#endif /* defined(__noctisgames__PooledObjectsManager__) */
