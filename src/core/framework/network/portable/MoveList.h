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

class IInputState;

class MoveList
{
public:
    MoveList();
    
    const Move& addMove(IInputState* inInputState, float inTimestamp);
    
    bool addMoveIfNew(const Move& inMove);
    
    void markMoveAsProcessed(Move* move);
    
    void removeProcessedMoves(float inLastMoveProcessedOnServerTimestamp);
    
    float getLastMoveTimestamp() const;
    
    float getLastProcessedMoveTimestamp() const;
    
    const Move& getLatestMove() const;
    
    void clear();
    
    bool hasMoves() const;
    
    int getMoveCount() const;
    
    Move* getMoveAtIndex(int index);
    
    std::deque<Move>::const_iterator begin() const;
    
    std::deque<Move>::const_iterator end() const;
    
private:
    float m_fLastMoveTimestamp;
    float m_fLastProcessedMoveTimestamp;
    std::deque<Move> m_moves;
};

#endif /* defined(__noctisgames__MoveList__) */
