//
//  Timing.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Timing__
#define __noctisgames__Timing__

#include <unistd.h>

class Timing
{
public:
    static Timing* getInstance();
    
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
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Timing();
    Timing(const Timing&);
    Timing& operator=(const Timing&);
};

#endif /* defined(__noctisgames__Timing__) */
