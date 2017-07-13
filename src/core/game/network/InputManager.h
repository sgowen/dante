//
//  InputManager.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputManager__
#define __noctisgames__InputManager__

#include "MoveList.h"

#include <string>

class InputState;
class Move;

#define INPUT_MANAGER_CALLBACKS InputManager::sRemoveProcessedMoves, InputManager::sGetMoveList, InputManager::sOnPlayerWelcomed

class InputManager
{
public:
    static InputManager* getInstance();
    
    static void sRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp);
    
    static MoveList& sGetMoveList();
    
    static void sOnPlayerWelcomed(uint8_t playerId);
    
    void update();
    
    const Move* getPendingMove();
    
    void clearPendingMove();
    
    void setConnected(bool isConnected);
    
    void setLiveMode(bool isLiveMode);
    
    void resetLiveInput();
    
    bool isLiveMode();
    
    bool isTimeToProcessInput();
    
    InputState* getInputState();
    
    MoveList& getMoveList();
    
    std::string& getLiveInputRef();
    
    std::string getLiveInput();
    
private:
    std::string m_liveInput;
    InputState* m_currentState;
    MoveList m_moveList;
    const Move* m_pendingMove;
    float m_fNextTimeToSampleInput;
    bool m_isConnected;
    bool m_isLiveMode;
    bool m_isTimeToProcessInput;
    
    const Move& sampleInputAsMove();
    
    bool isTimeToSampleInput();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    InputManager();
    ~InputManager();
    InputManager(const InputManager&);
    InputManager& operator=(const InputManager&);
};

#endif /* defined(__noctisgames__InputManager__) */
