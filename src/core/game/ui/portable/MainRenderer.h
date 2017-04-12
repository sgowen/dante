//
//  MainRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainRenderer__
#define __dante__MainRenderer__

#include "Renderer.h"

class MainRenderer : public Renderer
{
public:
    MainRenderer();
    
    virtual ~MainRenderer();
    
    virtual void createDeviceDependentResources();
    
    virtual void releaseDeviceDependentResources();
    
    virtual void beginFrame();
    
    void renderToScreen();
};

#endif /* defined(__dante__MainRenderer__) */
