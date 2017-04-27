//
//  TransitionGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/27/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "TransitionGpuProgramWrapper.h"

#include "GpuTextureWrapper.h"

TransitionGpuProgramWrapper::TransitionGpuProgramWrapper() : m_to(nullptr), m_fProgress(0)
{
    // Empty
}

TransitionGpuProgramWrapper::~TransitionGpuProgramWrapper()
{
    // Override
}

void TransitionGpuProgramWrapper::configure(GpuTextureWrapper* to, float progress)
{
    m_to = to;
    m_fProgress = progress;
}
