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
    
    m_fDeltaTime = (float) (currentTime - m_dLastFrameStartTime);
    
    m_dLastFrameStartTime = currentTime;
    m_fFrameStartTimef = static_cast<float> (m_dLastFrameStartTime);
}

void Timing::updateManual(float stateTime, float deltaTime)
{
    m_fDeltaTime = deltaTime;
    
    m_dLastFrameStartTime = stateTime;
    m_fFrameStartTimef = stateTime;
}

float Timing::getDeltaTime() const
{
    return m_fDeltaTime;
}

float Timing::getFrameStartTime() const
{
    return m_fFrameStartTimef;
}

float Timing::getTime() const
{
#if _WIN32
    LARGE_INTEGER curTime, timeSinceStart;
    QueryPerformanceCounter(&curTime);
    
    timeSinceStart.QuadPart = curTime.QuadPart - sStartTime.QuadPart;
    
    return timeSinceStart.QuadPart * m_dPerfCountDuration;
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
    m_dPerfCountDuration = 1.0 / perfFreq.QuadPart;
    
    QueryPerformanceCounter(&sStartTime);
#else
    sStartTime = steady_clock::now();
#endif
    
    m_dLastFrameStartTime = getTime();
}
