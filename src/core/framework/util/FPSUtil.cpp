//
//  FPSUtil.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/6/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "FPSUtil.h"

FPSUtil* FPSUtil::getInstance()
{
    static FPSUtil instance = FPSUtil();
    return &instance;
}

void FPSUtil::update(float deltaTime)
{
    _stateTime += deltaTime;
    ++m_iNumFrames;
    
    if (_stateTime > 1)
    {
        m_iFPS = m_iNumFrames;
        
        _stateTime = 0;
        m_iNumFrames = 0;
    }
}

int FPSUtil::getFPS() const
{
    return m_iFPS;
}

FPSUtil::FPSUtil() : _stateTime(0), m_iFPS(0), m_iNumFrames(0)
{
    // Empty
}

FPSUtil::~FPSUtil()
{
    // Empty
}
