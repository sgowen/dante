//
//  WeightedTimedMovingAverage.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "WeightedTimedMovingAverage.h"

#include "Timing.h"
#include "MathUtil.h"

WeightedTimedMovingAverage::WeightedTimedMovingAverage(float inDuration) : m_fDuration(inDuration), m_fValue(0.f)
{
    m_fTimeLastEntryMade = Timing::getInstance()->GetTime();
}

void WeightedTimedMovingAverage::UpdatePerSecond(float inValue)
{
    float time = Timing::getInstance()->GetTime();
    float timeSinceLastEntry = clamp(time - m_fTimeLastEntryMade, 10, 0);
    
    float valueOverTime = inValue / timeSinceLastEntry;
    
    // now update our value by whatever amount of the duration that was..
    float fractionOfDuration  = (timeSinceLastEntry / m_fDuration);
    if (fractionOfDuration > 1.0f)
    {
        fractionOfDuration = 1.0f;
    }
    
    m_fValue = m_fValue * (1.0f - fractionOfDuration) + valueOverTime * fractionOfDuration;
    
    m_fTimeLastEntryMade = time;
}

void WeightedTimedMovingAverage::Update(float inValue)
{
    float time = Timing::getInstance()->GetTime();
    float timeSinceLastEntry = clamp(time - m_fTimeLastEntryMade, 10, 0);
    
    // now update our value by whatever amount of the duration that was..
    float fractionOfDuration  = (timeSinceLastEntry / m_fDuration);
    if (fractionOfDuration > 1.0f)
    {
        fractionOfDuration = 1.0f;
    }
    
    m_fValue = m_fValue * (1.0f - fractionOfDuration) + inValue * fractionOfDuration;
    
    m_fTimeLastEntryMade = time;
}

float WeightedTimedMovingAverage::GetValue() const
{
    return m_fValue;
}
