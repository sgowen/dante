//
//  Timing.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Timing__
#define __noctisgames__Timing__

#ifndef _WIN32
#include <chrono>
#endif

class Timing
{
public:
    static Timing* getInstance();
    
    void update();
    
    void updateManual(float stateTime, float deltaTime);
    
    void setDeltaTime(float inDeltaTime);
    
    float getDeltaTime() const;
    
    float getFrameStartTime() const;
    
private:
#ifdef _WIN32
    LARGE_INTEGER _startTime = { 0 };
#else
    std::chrono::steady_clock::time_point _startTime;
#endif
    
    double _lastFrameStartTime;
    double _perfCountDuration;
    float _frameStartTime;
    float _deltaTime;
    
    float getTime() const;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Timing();
    ~Timing();
    Timing(const Timing&);
    Timing& operator=(const Timing&);
};

#endif /* defined(__noctisgames__Timing__) */
