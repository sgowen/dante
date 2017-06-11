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

class InputState;
class Move;

class InputManager
{
public:
    static InputManager* getInstance();
    
    void update();
    
    MoveList& getMoveList();
    
    const Move* getAndClearPendingMove();
    
private:
    InputState* m_currentState;
    MoveList m_moveList;
    const Move* m_pendingMove;
    float m_fNextTimeToSampleInput;
    
    const Move& sampleInputAsMove();
    
    bool isTimeToSampleInput();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    InputManager();
    ~InputManager();
    InputManager(const InputManager&);
    InputManager& operator=(const InputManager&);
};

#endif /* defined(__noctisgames__InputManager__) */
