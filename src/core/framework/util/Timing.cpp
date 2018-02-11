//
//  Timing.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/util/Timing.h>

Timing* Timing::getInstance()
{
    static Timing instance = Timing();
    return &instance;
}

void Timing::setTime(float stateTime)
{
    _frameStartTime = stateTime;
}

float Timing::getTime() const
{
    return _frameStartTime;
}

Timing::Timing() : _frameStartTime(0)
{
    // Empty
}

Timing::~Timing()
{
    // Empty
}
