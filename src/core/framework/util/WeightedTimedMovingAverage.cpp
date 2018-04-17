//
//  WeightedTimedMovingAverage.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/util/WeightedTimedMovingAverage.h>

#include <framework/util/Timing.h>

#include <framework/util/MathUtil.h>

WeightedTimedMovingAverage::WeightedTimedMovingAverage(Timing* timing, float inDuration) : _timing(timing), _duration(inDuration), _value(0.f)
{
    assert(_timing);
    
    _timeLastEntryMade = _timing->getTime();
}

void WeightedTimedMovingAverage::updatePerSecond(float inValue)
{
    if (inValue < 0)
    {
        return;
    }
    
    float time = _timing->getTime();
    float timeSinceLastEntry = clamp(time - _timeLastEntryMade, 0, 10);
    
    float valueOverTime = inValue / timeSinceLastEntry;
    
    // now update our value by whatever amount of the duration that was..
    float fractionOfDuration  = (timeSinceLastEntry / _duration);
    if (fractionOfDuration > 1.0f)
    {
        fractionOfDuration = 1.0f;
    }
    
    _value = _value * (1.0f - fractionOfDuration) + valueOverTime * fractionOfDuration;
    
    _timeLastEntryMade = time;
}

void WeightedTimedMovingAverage::update(float inValue)
{
    if (inValue < 0)
    {
        return;
    }
    
    float time = _timing->getTime();
    float timeSinceLastEntry = clamp(time - _timeLastEntryMade, 0, 10);
    
    // now update our value by whatever amount of the duration that was..
    float fractionOfDuration  = (timeSinceLastEntry / _duration);
    if (fractionOfDuration > 1.0f)
    {
        fractionOfDuration = 1.0f;
    }
    
    _value = _value * (1.0f - fractionOfDuration) + inValue * fractionOfDuration;
    
    _timeLastEntryMade = time;
}

float WeightedTimedMovingAverage::getValue() const
{
    return _value;
}
