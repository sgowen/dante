//
//  WeightedTimedMovingAverage.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__WeightedTimedMovingAverage__
#define __noctisgames__WeightedTimedMovingAverage__

class Timing;

class WeightedTimedMovingAverage
{
public:
    WeightedTimedMovingAverage(Timing* timing, float inDuration = 5.f);
    
    void updatePerSecond(float inValue);
    void update(float inValue);
    float getValue() const;
    
private:
    Timing* _timing;
    float _timeLastEntryMade;
    float _value;
    float _duration;
};

#endif /* defined(__noctisgames__WeightedTimedMovingAverage__) */
