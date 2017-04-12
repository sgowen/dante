//
//  OpenGLDeviceHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLDeviceHelper__
#define __noctisgames__OpenGLDeviceHelper__

#include "IDeviceHelper.h"

class OpenGLDeviceHelper : public IDeviceHelper
{
public:
    OpenGLDeviceHelper();
    
    virtual ~OpenGLDeviceHelper();
    
    virtual void createDeviceDependentResources(int maxBatchSize);
    
    virtual void createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers);
    
    virtual void releaseDeviceDependentResources();
};

#endif /* defined(__noctisgames__OpenGLDeviceHelper__) */
