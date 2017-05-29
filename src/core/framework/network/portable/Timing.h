//
//  Timing.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Timing__
#define __noctisgames__Timing__

#include "NetworkConstants.h"

class Timing
{
public:
    static Timing sInstance;
    
    Timing();
    
    void Update();
    
    void updateManual(float stateTime, float deltaTime);
    
    float GetTime() const;
    
    float GetDeltaTime() const;
    
    float GetFrameStartTime() const;
    
private:
    float mDeltaTime;
    uint64_t mDeltaTick;
    
    double mLastFrameStartTime;
    float mFrameStartTimef;
    double mPerfCountDuration;
};

#endif /* defined(__noctisgames__Timing__) */
