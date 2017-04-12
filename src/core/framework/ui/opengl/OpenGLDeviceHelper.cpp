//
//  OpenGLDeviceHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLDeviceHelper.h"

#include "OpenGLManager.h"

OpenGLDeviceHelper::OpenGLDeviceHelper() : IDeviceHelper()
{
    OpenGLManager::create();
}

OpenGLDeviceHelper::~OpenGLDeviceHelper()
{
    OpenGLManager::destroy();
}

void OpenGLDeviceHelper::createDeviceDependentResources(int maxBatchSize)
{
    OGLManager->createDeviceDependentResources(maxBatchSize);
}

void OpenGLDeviceHelper::createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers)
{
    OGLManager->createWindowSizeDependentResources(renderWidth, renderHeight, numFramebuffers);
}

void OpenGLDeviceHelper::releaseDeviceDependentResources()
{
    OGLManager->releaseDeviceDependentResources();
}
