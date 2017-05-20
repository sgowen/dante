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
mDuration(inDuration),
mValue(0.f)
{
    mTimeLastEntryMade = Timing::sInstance.GetTimef();
}

void WeightedTimedMovingAverage::UpdatePerSecond(float inValue)
{
    float time = Timing::sInstance.GetTimef();
    float timeSinceLastEntry = time - mTimeLastEntryMade;
    
    float valueOverTime = inValue / timeSinceLastEntry;
    
    //now update our value by whatever amount of the duration that was..
    float fractionOfDuration  = (timeSinceLastEntry / mDuration);
    if (fractionOfDuration > 1.f) { fractionOfDuration = 1.f; }
    
    mValue = mValue * (1.f - fractionOfDuration) + valueOverTime * fractionOfDuration;
    
    mTimeLastEntryMade = time;
}

void WeightedTimedMovingAverage::Update(float inValue)
{
    float time = Timing::sInstance.GetTimef();
    float timeSinceLastEntry = time - mTimeLastEntryMade;
    
    //now update our value by whatever amount of the duration that was..
    float fractionOfDuration  = (timeSinceLastEntry / mDuration);
    if (fractionOfDuration > 1.f) { fractionOfDuration = 1.f; }
    
    mValue = mValue * (1.f - fractionOfDuration) + inValue * fractionOfDuration;
    
    mTimeLastEntryMade = time;
} 

float WeightedTimedMovingAverage::GetValue() const
{
    return mValue;
}
