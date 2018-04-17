//
//  FPSUtil.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 8/6/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/util/FPSUtil.h>

FPSUtil* FPSUtil::getInstance()
{
    static FPSUtil instance = FPSUtil();
    return &instance;
}

void FPSUtil::update(double deltaTime)
{
    _stateTime += deltaTime;
    ++_numFrames;
    
    if (_stateTime >= 1.0)
    {
        _fPS = _numFrames;
        
        _stateTime = 0;
        _numFrames = 0;
    }
}

int FPSUtil::getFPS() const
{
    return _fPS;
}

FPSUtil::FPSUtil() : _stateTime(0), _fPS(0), _numFrames(0)
{
    // Empty
}

FPSUtil::~FPSUtil()
{
    // Empty
}
