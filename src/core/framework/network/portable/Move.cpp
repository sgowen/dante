//
//  Move.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/network/portable/Move.h"

#include "framework/network/portable/InputState.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"

Move::Move(InputState* inInputState, float inTimestamp) :
_inputState(inInputState),
_timestamp(inTimestamp)
{
    // Empty
}

Move::Move(InputState* inInputState) :
_inputState(inInputState),
_timestamp(0)
{
    // Empty
}

Move::~Move()
{
    // Empty
}

bool Move::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write(_timestamp);
    
    return _inputState->write(inOutputStream);
}

bool Move::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(_timestamp);
    
    return _inputState->read(inInputStream);
}

bool Move::isEqual(const Move* inMove) const
{
    return _inputState->isEqual(inMove->getInputState());
}

InputState* Move::getInputState() const
{
    return _inputState;
}

float Move::getTimestamp() const
{
    return _timestamp;
}

void Move::setTimestamp(float timeStamp)
{
    _timestamp = timeStamp;
}

void Move::copyInputState(InputState* inInputState)
{
    inInputState->copyTo(_inputState);
}
