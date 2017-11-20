//
//  TransitionGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/27/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/TransitionGpuProgramWrapper.h"

#include "framework/graphics/portable/GpuTextureWrapper.h"

#include <memory>

TransitionGpuProgramWrapper::TransitionGpuProgramWrapper() : _to(NULL), _progress(0)
{
    // Empty
}

TransitionGpuProgramWrapper::~TransitionGpuProgramWrapper()
{
    // Override
}

void TransitionGpuProgramWrapper::configure(GpuTextureWrapper* to, float progress)
{
    _to = to;
    _progress = progress;
}
