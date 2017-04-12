//
//  GamePadInputManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GamePadInputManager__
#define __noctisgames__GamePadInputManager__

#include "GamePadEventType.h"
#include "NGRollingPool.h"

#include <vector>

#define GAME_PAD_INPUT_MANAGER (GamePadInputManager::getInstance())

class GamePadEvent;

class GamePadInputManager
{
public:
    static GamePadInputManager* getInstance();
    
    void onInput(GamePadEventType type, int index, float x = 0, float y = 0);
    
    void process();
    
    std::vector<GamePadEvent*>& getEvents();
    
private:
    NGRollingPool<GamePadEvent>* m_pool;
    
    void addEvent(GamePadEventType type, int index, float x, float y);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    GamePadInputManager();
    GamePadInputManager(const GamePadInputManager&);
    GamePadInputManager& operator=(const GamePadInputManager&);
};

#endif /* defined(__noctisgames__GamePadInputManager__) */
