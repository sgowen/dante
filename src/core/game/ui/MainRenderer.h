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

class TextureWrapper;

class MainRenderer : public Renderer
{
public:
    MainRenderer(int maxBatchSize);
    
    virtual ~MainRenderer();
    
    virtual void createDeviceDependentResources();
    
    virtual void releaseDeviceDependentResources();
    
    void tempDraw(float stateTime);
    
private:
    TextureWrapper* m_demo;
};

#endif /* defined(__dante__MainRenderer__) */
