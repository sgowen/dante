//
//  Timing.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/util/Timing.h"

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
    _frameStartTime = static_cast<float> (_lastFrameStartTime);
}

void Timing::updateManual(float stateTime, float deltaTime)
{
    _deltaTime = deltaTime;
    
    _lastFrameStartTime = stateTime;
    _frameStartTime = stateTime;
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
    return _frameStartTime;
}

float Timing::getTime() const
{
#ifdef _WIN32
    LARGE_INTEGER curTime, timeSinceStart;
    QueryPerformanceCounter(&curTime);
    
    timeSinceStart.QuadPart = curTime.QuadPart - _startTime.QuadPart;
    
    return timeSinceStart.QuadPart * _perfCountDuration;
#else
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime).count();
    
    return static_cast<float>(ms) / 1000;
#endif
}

Timing::Timing()
{
#ifdef _WIN32
    LARGE_INTEGER perfFreq;
    QueryPerformanceFrequency(&perfFreq);
    _perfCountDuration = 1.0 / perfFreq.QuadPart;
    
    QueryPerformanceCounter(&_startTime);
#else
    _startTime = std::chrono::steady_clock::now();
#endif
    
    _lastFrameStartTime = getTime();
    _frameStartTime = static_cast<float> (_lastFrameStartTime);
}

Timing::~Timing()
{
    // Empty
}
