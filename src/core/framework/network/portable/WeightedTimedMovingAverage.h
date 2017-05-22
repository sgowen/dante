//
//  WeightedTimedMovingAverage.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__WeightedTimedMovingAverage__
#define __noctisgames__WeightedTimedMovingAverage__

class WeightedTimedMovingAverage
{
public:
    WeightedTimedMovingAverage(float inDuration = 5.f);
    
    void UpdatePerSecond(float inValue);
    
    void Update(float inValue);
    
    float GetValue() const;
    
private:
    float mTimeLastEntryMade;
    float mValue;
    float mDuration;
};

#endif /* defined(__noctisgames__WeightedTimedMovingAverage__) */
