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

class InputManager
{
public:
    static InputManager* getInstance();
    
    static void staticRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp);
    
    static MoveList& staticGetMoveList();
    
    void update();
    
    const Move* getAndClearPendingMove();
    
    void setConnected(bool isConnected);
    
    void setLiveMode(bool isLiveMode);
    
    void resetLiveInput();
    
    bool isLiveMode();
    
    bool isTimeToProcessInput();
    
    bool isEscapeKeyPressed();
    
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
    bool m_isEscapeKeyPressed;
    
    const Move& sampleInputAsMove();
    
    bool isTimeToSampleInput();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    InputManager();
    ~InputManager();
    InputManager(const InputManager&);
    InputManager& operator=(const InputManager&);
};

#endif /* defined(__noctisgames__InputManager__) */
