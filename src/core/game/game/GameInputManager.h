//
//  GameInputManager.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameInputManager__
#define __noctisgames__GameInputManager__

#include <framework/network/portable/MoveList.h>
#include <framework/util/NGPool.h>
#include <game/game/GameInputState.h>

#include <string>

class Move;
class GameEngine;

#define INPUT_MANAGER_CALLBACKS GameInputManager::sRemoveProcessedMoves, GameInputManager::sGetMoveList, GameInputManager::sOnPlayerWelcomed

enum GameInputManagerState
{
    GIMS_NONE = 0,
    GIMS_ESCAPE
};

class GameInputManager
{
public:
    static void create(GameEngine* engine);
    static GameInputManager* getInstance();
    static void destroy();
    
    static void sRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp);
    static MoveList& sGetMoveList();
    static void sOnPlayerWelcomed(uint8_t playerId);
    static void sHandleInputStateRelease(InputState* inputState);
    
    void update();
    const Move* getPendingMove();
    void clearPendingMove();
    GameInputState* getInputState();
    MoveList& getMoveList();
    int getMenuState();
    
private:
    static GameInputManager* s_instance;
    
    NoctisGames::NGPool<GameInputState> _inputStates;
    GameInputState* _currentState;
    MoveList _moveList;
    const Move* _pendingMove;
    int _inputState;
    bool _isTimeToProcessInput;
    GameEngine* _engine;
    float _playerLightZDelta;
    
    const Move& sampleInputAsMove();
    
    void dropPlayer(int index);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    GameInputManager(GameEngine* engine);
    ~GameInputManager();
    GameInputManager(const GameInputManager&);
    GameInputManager& operator=(const GameInputManager&);
};

#endif /* defined(__noctisgames__GameInputManager__) */
