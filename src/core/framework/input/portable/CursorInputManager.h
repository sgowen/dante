//
//  CursorInputManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__CursorInputManager__
#define __noctisgames__CursorInputManager__

#include "CursorEventType.h"
#include "NGRollingPool.h"

#include <vector>

#define SCREEN_INPUT_MANAGER (CursorInputManager::getInstance())

class CursorEvent;

class CursorInputManager
{
public:
    static CursorInputManager* getInstance();
    
    void onTouch(CursorEventType type, float x, float y);
    
    void process();
    
    std::vector<CursorEvent*>& getEvents();
    
private:
    NGRollingPool<CursorEvent>* _pool;
    
    void addEvent(CursorEventType type, float x, float y);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    CursorInputManager();
    ~CursorInputManager();
    CursorInputManager(const CursorInputManager&);
    CursorInputManager& operator=(const CursorInputManager&);
};

#endif /* defined(__noctisgames__CursorInputManager__) */
