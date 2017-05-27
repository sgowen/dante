//
//  Timing.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Timing.h"

#if !_WIN32
#include <chrono>
using namespace std::chrono;
#endif

Timing Timing::sInstance;

namespace
{
#if _WIN32
    LARGE_INTEGER sStartTime = { 0 };
#else
    steady_clock::time_point sStartTime;
#endif
}

Timing::Timing()
{
#if _WIN32
    LARGE_INTEGER perfFreq;
    QueryPerformanceFrequency(&perfFreq);
    mPerfCountDuration = 1.0 / perfFreq.QuadPart;
    
    QueryPerformanceCounter(&sStartTime);
#else
    sStartTime = steady_clock::now();
#endif
    
    mLastFrameStartTime = GetTime();
}

void Timing::Update()
{
    double currentTime = GetTime();
    
    mDeltaTime = (float) (currentTime - mLastFrameStartTime);
    
    mLastFrameStartTime = currentTime;
    mFrameStartTimef = static_cast<float> (mLastFrameStartTime);
}

void Timing::updateManual(float stateTime, float deltaTime)
{
    mDeltaTime = deltaTime;
    
    mLastFrameStartTime = stateTime;
    mFrameStartTimef = static_cast<float> (mLastFrameStartTime);
}

float Timing::GetTime() const
{
#if _WIN32
    LARGE_INTEGER curTime, timeSinceStart;
    QueryPerformanceCounter(&curTime);
    
    timeSinceStart.QuadPart = curTime.QuadPart - sStartTime.QuadPart;
    
    return timeSinceStart.QuadPart * mPerfCountDuration;
#else
    auto now = steady_clock::now();
    auto ms = duration_cast< milliseconds >(now - sStartTime).count();
    
    return static_cast<float>(ms) / 1000;
#endif
}
