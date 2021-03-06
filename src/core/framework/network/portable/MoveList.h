//
//  MoveList.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MoveList__
#define __noctisgames__MoveList__

#include <framework/network/portable/Move.h>

#include <deque>

typedef void (*InputStateReleaseFunc)(InputState* inputState);

class InputState;

class MoveList
{
public:
    MoveList();
    
    const Move& addMove(InputState* inInputState, float inTimestamp);
    bool addMoveIfNew(const Move& inMove);
    void markMoveAsProcessed(Move* move);
    void removeProcessedMoves(float inLastMoveProcessedOnServerTimestamp, InputStateReleaseFunc inputStateReleaseFunc);
    
    float getLastMoveTimestamp() const;
    float getLastProcessedMoveTimestamp() const;
    const Move& getLatestMove() const;
    void clear();
    bool hasMoves() const;
    int getMoveCount() const;
    int getNumMovesAfterTimestamp(float inLastMoveReceivedOnServerTimestamp) const;
    Move* getMoveAtIndex(int index);
    
    std::deque<Move>::const_iterator begin() const;
    std::deque<Move>::const_iterator end() const;
    
private:
    float _lastMoveTimestamp;
    float _lastProcessedMoveTimestamp;
    std::deque<Move> _moves;
};

#endif /* defined(__noctisgames__MoveList__) */
