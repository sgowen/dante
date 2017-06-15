//
//  IInputState.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "IInputState.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

IInputState::IInputState() : m_isInUse(false)
{
    // Empty
}

IInputState::~IInputState()
{
    // Empty
}

void IInputState::setInUse(bool inUse)
{
    m_isInUse = inUse;
}

bool IInputState::isInUse() const
{
    return m_isInUse;
}

RTTI_IMPL_NOPARENT(IInputState);
