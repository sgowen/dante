//
//  MoveList.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MoveList.h"

#include "IInputState.h"

MoveList::MoveList(): m_fLastMoveTimestamp(-1.f)
{
    // Empty
}

const Move& MoveList::addMove(IInputState* inInputState, float inTimestamp)
{
    //first move has 0 time. it's okay, it only happens once
    float deltaTime = m_fLastMoveTimestamp >= 0.f ? inTimestamp - m_fLastMoveTimestamp : 0.f;
    
    m_moves.emplace_back(inInputState, inTimestamp, deltaTime);
    
    m_fLastMoveTimestamp = inTimestamp;
    
    return m_moves.back();
}

bool MoveList::addMoveIfNew(const Move& inMove)
{
    //we might have already received this move in another packet (since we're sending the same move in multiple packets)
    //so make sure it's new...
    
    //adjust the deltatime and then place!
    float timeStamp = inMove.getTimestamp();
    
    if (timeStamp > m_fLastMoveTimestamp)
    {
        float deltaTime = m_fLastMoveTimestamp >= 0.f ? timeStamp - m_fLastMoveTimestamp : 0.f;
        
        m_fLastMoveTimestamp = timeStamp;
        
        m_moves.emplace_back(inMove.getInputState(), timeStamp, deltaTime);
        
        return true;
    }
    
    return false;
}

void MoveList::removedProcessedMoves(float inLastMoveProcessedOnServerTimestamp)
{
    while (!m_moves.empty() && m_moves.front().getTimestamp() <= inLastMoveProcessedOnServerTimestamp)
    {
        m_moves.pop_front();
    }
}

float MoveList::getLastMoveTimestamp() const
{
    return m_fLastMoveTimestamp;
}

const Move& MoveList::getLatestMove() const
{
    return m_moves.back();
}

void MoveList::clear()
{
    m_moves.clear();
}

bool MoveList::hasMoves() const
{
    return !m_moves.empty();
}

int MoveList::getMoveCount() const
{
    return static_cast<int>(m_moves.size());
}

std::deque<Move>::const_iterator MoveList::begin() const
{
    return m_moves.begin();
}

std::deque<Move>::const_iterator MoveList::end() const
{
    return m_moves.end();
}
