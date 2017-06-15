//
//  Move.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Move.h"

#include "IInputState.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

Move::Move(IInputState* inInputState, float inTimestamp, float inDeltaTime) :
m_inputState(inInputState),
m_fTimestamp(inTimestamp),
m_fDeltaTime(inDeltaTime)
{
    // Empty
}

Move::Move(IInputState* inInputState) :
m_inputState(inInputState),
m_fTimestamp(0),
m_fDeltaTime(0)
{
    // Empty
}

Move::~Move()
{
    m_inputState->setInUse(false);
}

bool Move::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write(m_fTimestamp);
    
    m_inputState->write(inOutputStream);
    
    return true;
}

bool Move::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(m_fTimestamp);
    
    m_inputState->read(inInputStream);
    
    return true;
}

IInputState* Move::getInputState() const
{
    return m_inputState;
}

float Move::getTimestamp() const
{
    return m_fTimestamp;
}

float Move::getDeltaTime() const
{
    return m_fDeltaTime;
}
