//
//  ScreenInputManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ScreenInputManager__
#define __noctisgames__ScreenInputManager__

#include "ScreenEventType.h"
#include "NGRollingPool.h"

#include <vector>

#define SCREEN_INPUT_MANAGER (ScreenInputManager::getInstance())

class ScreenEvent;

class ScreenInputManager
{
public:
    static ScreenInputManager* getInstance();
    
    void onTouch(ScreenEventType type, float x, float y);
    
    void process();
    
    std::vector<ScreenEvent*>& getEvents();
    
private:
    NGRollingPool<ScreenEvent>* m_pool;
    
    void addEvent(ScreenEventType type, float x, float y);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    ScreenInputManager();
    ScreenInputManager(const ScreenInputManager&);
    ScreenInputManager& operator=(const ScreenInputManager&);
};

#endif /* defined(__noctisgames__ScreenInputManager__) */
