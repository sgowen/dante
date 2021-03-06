//
//  MoveList.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/MoveList.h>

#include <framework/network/portable/InputState.h>
#include <framework/util/Timing.h>
#include <framework/util/StringUtil.h>

MoveList::MoveList(): _lastMoveTimestamp(-1.0f), _lastProcessedMoveTimestamp(-1.0f)
{
    // Empty
}

const Move& MoveList::addMove(InputState* inInputState, float inTimestamp)
{
    _moves.push_back(Move(inInputState, inTimestamp));
    
    _lastMoveTimestamp = inTimestamp;
    
    return _moves.back();
}

bool MoveList::addMoveIfNew(const Move& inMove)
{
    //we might have already received this move in another packet (since we're sending the same move in multiple packets)
    //so make sure it's new...
    
    //adjust the deltatime and then place!
    float timeStamp = inMove.getTimestamp();
    
    if (timeStamp > _lastMoveTimestamp)
    {
        _lastMoveTimestamp = timeStamp;
        
        _moves.push_back(Move(inMove.getInputState(), timeStamp));
        
        return true;
    }
    
    return false;
}

void MoveList::markMoveAsProcessed(Move* move)
{
    _lastProcessedMoveTimestamp = move->getTimestamp();
}

void MoveList::removeProcessedMoves(float inLastMoveProcessedOnServerTimestamp, InputStateReleaseFunc inputStateReleaseFunc)
{
    while (!_moves.empty() && _moves.front().getTimestamp() <= inLastMoveProcessedOnServerTimestamp)
    {
        inputStateReleaseFunc(_moves.front().getInputState());

        _moves.pop_front();
    }
}

float MoveList::getLastMoveTimestamp() const
{
    return _lastMoveTimestamp;
}

float MoveList::getLastProcessedMoveTimestamp() const
{
    return _lastProcessedMoveTimestamp;
}

const Move& MoveList::getLatestMove() const
{
    return _moves.back();
}

void MoveList::clear()
{
    _moves.clear();
}

bool MoveList::hasMoves() const
{
    return !_moves.empty();
}

int MoveList::getMoveCount() const
{
    return static_cast<int>(_moves.size());
}

int MoveList::getNumMovesAfterTimestamp(float inLastMoveReceivedOnServerTimestamp) const
{
    int ret = 0;
    
    for (Move move : _moves)
    {
        if (move.getTimestamp() > inLastMoveReceivedOnServerTimestamp)
        {
            ++ret;
        }
    }
    
    return ret;
}

Move* MoveList::getMoveAtIndex(int index)
{
    if (index < getMoveCount())
    {
        return &_moves.at(index);
    }
    
    return NULL;
}

std::deque<Move>::const_iterator MoveList::begin() const
{
    return _moves.begin();
}

std::deque<Move>::const_iterator MoveList::end() const
{
    return _moves.end();
}
