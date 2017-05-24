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

WeightedTimedMovingAverage::WeightedTimedMovingAverage(float inDuration) :
m_fDuration(inDuration),
m_fValue(0.f)
{
    m_fTimeLastEntryMade = Timing::sInstance.GetTimef();
}

void WeightedTimedMovingAverage::UpdatePerSecond(float inValue)
{
    float time = Timing::sInstance.GetTimef();
    float timeSinceLastEntry = time - m_fTimeLastEntryMade;
    
    float valueOverTime = inValue / timeSinceLastEntry;
    
    //now update our value by whatever amount of the duration that was..
    float fractionOfDuration  = (timeSinceLastEntry / m_fDuration);
    if (fractionOfDuration > 1.f) { fractionOfDuration = 1.f; }
    
    inValue = m_fValue * (1.f - fractionOfDuration) + valueOverTime * fractionOfDuration;
    
    m_fTimeLastEntryMade = time;
}

void WeightedTimedMovingAverage::Update(float inValue)
{
    float time = Timing::sInstance.GetTimef();
    float timeSinceLastEntry = time - m_fTimeLastEntryMade;
    
    //now update our value by whatever amount of the duration that was..
    float fractionOfDuration  = (timeSinceLastEntry / m_fDuration);
    if (fractionOfDuration > 1.f) { fractionOfDuration = 1.f; }
    
    m_fValue = m_fValue * (1.f - fractionOfDuration) + inValue * fractionOfDuration;
    
    m_fTimeLastEntryMade = time;
}

float WeightedTimedMovingAverage::GetValue() const
{
    return m_fValue;
}
