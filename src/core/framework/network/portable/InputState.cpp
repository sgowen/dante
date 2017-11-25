//
//  InputState.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/network/portable/InputState.h"

#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"

NGRTTI_IMPL_NOPARENT(InputState);

InputState::InputState() : _isInUse(false)
{
    // Empty
}

InputState::~InputState()
{
    // Empty
}

void InputState::setInUse(bool inUse)
{
    _isInUse = inUse;
}

bool InputState::isInUse() const
{
    return _isInUse;
}
