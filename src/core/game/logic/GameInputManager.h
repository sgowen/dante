//
//  GameInputManager.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameInputManager__
#define __noctisgames__GameInputManager__

#include "framework/network/portable/MoveList.h"

#include <string>

class MainInputState;
class Move;

#define INPUT_MANAGER_CALLBACKS GameInputManager::sRemoveProcessedMoves, GameInputManager::sGetMoveList, GameInputManager::sOnPlayerWelcomed

class GameInputManager
{
public:
    static void create();
    static GameInputManager* getInstance();
    static void destroy();
    
    static void sRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp);
    static MoveList& sGetMoveList();
    static void sOnPlayerWelcomed(uint8_t playerId);
    
    void update();
    const Move* getPendingMove();
    void clearPendingMove();
    MainInputState* getInputState();
    MoveList& getMoveList();
    bool isPlayerIdLocalHost(uint8_t playerId);
    int getMenuState();
    
private:
    static GameInputManager* s_instance;
    
    MainInputState* _currentState;
    MoveList _moveList;
    const Move* _pendingMove;
    int _menuState;
    bool _isTimeToProcessInput;
    
    const Move& sampleInputAsMove();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    GameInputManager();
    GameInputManager(const GameInputManager&);
    GameInputManager& operator=(const GameInputManager&);
};

#endif /* defined(__noctisgames__GameInputManager__) */
