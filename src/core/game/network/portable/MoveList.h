//
//  MoveList.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MoveList__
#define __noctisgames__MoveList__

#include "Move.h"

#include <deque>

class InputState;

class MoveList
{
public:
    MoveList();
    
    const Move&	AddMove(const InputState& inInputState, float inTimestamp);
    
    bool AddMoveIfNew(const Move& inMove);
    
    void RemovedProcessedMoves(float inLastMoveProcessedOnServerTimestamp);
    
    float GetLastMoveTimestamp() const;
    
    const Move& GetLatestMove() const;
    
    void Clear();
    
    bool HasMoves() const;
    
    int GetMoveCount() const;
    
    std::deque<Move>::const_iterator begin() const;
    
    std::deque<Move>::const_iterator end() const;
    
private:
    float m_fLastMoveTimestamp;
    std::deque<Move> m_moves;
};

#endif /* defined(__noctisgames__MoveList__) */
