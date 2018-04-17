//
//  Timing.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/util/Timing.h>

#include <framework/util/Constants.h>

Timing::Timing() : _time(0)
{
    // Empty
}

Timing::~Timing()
{
    // Empty
}

void Timing::onFrame()
{
    _time += FRAME_RATE;
}

void Timing::reset()
{
    _time = 0;
}

float Timing::getTime() const
{
    return _time;
}
