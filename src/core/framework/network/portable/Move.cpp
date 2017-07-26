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

Move::Move(IInputState* inInputState, float inTimestamp) :
m_inputState(inInputState),
m_fTimestamp(inTimestamp)
{
    // Empty
}

Move::Move(IInputState* inInputState) :
m_inputState(inInputState),
m_fTimestamp(0)
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
    
    return m_inputState->write(inOutputStream);
}

bool Move::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(m_fTimestamp);
    
    return m_inputState->read(inInputStream);
}

IInputState* Move::getInputState() const
{
    return m_inputState;
}

float Move::getTimestamp() const
{
    return m_fTimestamp;
}
