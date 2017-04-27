//
//  DummyGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DummyGpuProgramWrapper.h"

DummyGpuProgramWrapper * DummyGpuProgramWrapper::getInstance()
{
    static DummyGpuProgramWrapper instance = DummyGpuProgramWrapper();
    return &instance;
}

DummyGpuProgramWrapper::~DummyGpuProgramWrapper()
{
    // Empty
}

void DummyGpuProgramWrapper::bind()
{
    // Empty
}

void DummyGpuProgramWrapper::unbind()
{
    // Empty
}

DummyGpuProgramWrapper::DummyGpuProgramWrapper()
{
    // Hide Constructor for Singleton
}
