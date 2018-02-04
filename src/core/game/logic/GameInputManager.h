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
class GameEngine;

#define INPUT_MANAGER_CALLBACKS GameInputManager::sRemoveProcessedMoves, GameInputManager::sGetMoveList, GameInputManager::sOnPlayerWelcomed

enum GameInputState
{
    GIS_NONE = 0,
    GIS_LOCAL_PLAYER_DROP_OUT_0,
    GIS_LOCAL_PLAYER_DROP_OUT_1,
    GIS_LOCAL_PLAYER_DROP_OUT_2,
    GIS_LOCAL_PLAYER_DROP_OUT_3,
    GIS_CLIENT_MAIN_TOGGLE_MUSIC,
    GIS_CLIENT_MAIN_TOGGLE_SOUND,
    GIS_TOGGLE_PHYSICS_DISPLAY,
    GIS_TOGGLE_INTERPOLATION,
    GIS_TOGGLE_LIGHTING,
    GIS_SERVER_TOGGLE_MAP
};

class GameInputManager
{
public:
    static void create();
    static GameInputManager* getInstance();
    static void destroy();
    
    static void sRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp);
    static MoveList& sGetMoveList();
    static void sOnPlayerWelcomed(uint8_t playerId);
    
    void setEngine(GameEngine* inValue);
    void update();
    const Move* getPendingMove();
    void clearPendingMove();
    MainInputState* getInputState();
    MoveList& getMoveList();
    int getMenuState();
    
private:
    static GameInputManager* s_instance;
    
    MainInputState* _currentState;
    MoveList _moveList;
    const Move* _pendingMove;
    int _inputState;
    bool _isTimeToProcessInput;
    GameEngine* _engine;
    float _playerLightZDelta;
    
    const Move& sampleInputAsMove();
    
    void dropPlayer(int index);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    GameInputManager();
    GameInputManager(const GameInputManager&);
    GameInputManager& operator=(const GameInputManager&);
};

#endif /* defined(__noctisgames__GameInputManager__) */
