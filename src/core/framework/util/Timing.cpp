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

namespace
{
#if _WIN32
    LARGE_INTEGER sStartTime = { 0 };
#else
    steady_clock::time_point sStartTime;
#endif
}

Timing* Timing::getInstance()
{
    static Timing instance = Timing();
    return &instance;
}

void Timing::update()
{
    double currentTime = getTime();
    
    _deltaTime = (float) (currentTime - _lastFrameStartTime);
    
    _lastFrameStartTime = currentTime;
    _frameStartTimef = static_cast<float> (_lastFrameStartTime);
}

void Timing::updateManual(float stateTime, float deltaTime)
{
    _deltaTime = deltaTime;
    
    _lastFrameStartTime = stateTime;
    _frameStartTimef = stateTime;
}

void Timing::setDeltaTime(float inDeltaTime)
{
    _deltaTime = inDeltaTime;
}

float Timing::getDeltaTime() const
{
    return _deltaTime;
}

float Timing::getFrameStartTime() const
{
    return _frameStartTimef;
}

float Timing::getTime() const
{
#if _WIN32
    LARGE_INTEGER curTime, timeSinceStart;
    QueryPerformanceCounter(&curTime);
    
    timeSinceStart.QuadPart = curTime.QuadPart - sStartTime.QuadPart;
    
    return timeSinceStart.QuadPart * _dPerfCountDuration;
#else
    auto now = steady_clock::now();
    auto ms = duration_cast< milliseconds >(now - sStartTime).count();
    
    return static_cast<float>(ms) / 1000;
#endif
}

Timing::Timing()
{
#if _WIN32
    LARGE_INTEGER perfFreq;
    QueryPerformanceFrequency(&perfFreq);
    _dPerfCountDuration = 1.0 / perfFreq.QuadPart;
    
    QueryPerformanceCounter(&sStartTime);
#else
    sStartTime = steady_clock::now();
#endif
    
    _lastFrameStartTime = getTime();
    _frameStartTimef = static_cast<float> (_lastFrameStartTime);
}

Timing::~Timing()
{
    // Empty
}
