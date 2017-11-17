//
//  PooledObjectsManager.h
//  dante
//
//  Created by Stephen Gowen on 6/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__PooledObjectsManager__
#define __noctisgames__PooledObjectsManager__

#include "NGRollingPool.h"

#include <vector>

#define POOLED_OBJ_MGR (PooledObjectsManager::getInstance())

class InputState;
class MainInputState;

class PooledObjectsManager
{
public:
    static PooledObjectsManager* getInstance();
    
    static InputState* borrowInputState();
    
private:
    NGRollingPool<MainInputState>* _pool;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    PooledObjectsManager();
    ~PooledObjectsManager();
    PooledObjectsManager(const PooledObjectsManager&);
    PooledObjectsManager& operator=(const PooledObjectsManager&);
};

#endif /* defined(__noctisgames__PooledObjectsManager__) */
