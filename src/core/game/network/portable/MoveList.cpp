//
//  MoveList.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MoveList.h"

#include "InputState.h"

MoveList::MoveList(): m_fLastMoveTimestamp(-1.f)
{
    // Empty
}

const Move& MoveList::AddMove(const InputState& inInputState, float inTimestamp)
{
    //first move has 0 time. it's okay, it only happens once
    float deltaTime = m_fLastMoveTimestamp >= 0.f ? inTimestamp - m_fLastMoveTimestamp : 0.f;
    
    m_moves.emplace_back(inInputState, inTimestamp, deltaTime);
    
    m_fLastMoveTimestamp = inTimestamp;
    
    return m_moves.back();
}

bool MoveList::AddMoveIfNew(const Move& inMove)
{
    //we might have already received this move in another packet (since we're sending the same move in multiple packets)
    //so make sure it's new...
    
    //adjust the deltatime and then place!
    float timeStamp = inMove.GetTimestamp();
    
    if (timeStamp > m_fLastMoveTimestamp)
    {
        float deltaTime = m_fLastMoveTimestamp >= 0.f ? timeStamp - m_fLastMoveTimestamp : 0.f;
        
        m_fLastMoveTimestamp = timeStamp;
        
        m_moves.emplace_back(inMove.GetInputState(), timeStamp, deltaTime);
        
        return true;
    }
    
    return false;
}

void MoveList::RemovedProcessedMoves(float inLastMoveProcessedOnServerTimestamp)
{
    while (!m_moves.empty() && m_moves.front().GetTimestamp() <= inLastMoveProcessedOnServerTimestamp)
    {
        m_moves.pop_front();
    }
}

float MoveList::GetLastMoveTimestamp() const
{
    return m_fLastMoveTimestamp;
}

const Move& MoveList::GetLatestMove() const
{
    return m_moves.back();
}

void MoveList::Clear()
{
    m_moves.clear();
}

bool MoveList::HasMoves() const
{
    return !m_moves.empty();
}

int MoveList::GetMoveCount() const
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
