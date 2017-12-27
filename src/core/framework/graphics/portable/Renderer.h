//
//  Renderer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Renderer__
#define __noctisgames__Renderer__

class Renderer
{
public:
    Renderer();
    virtual ~Renderer();
    
    virtual void createDeviceDependentResources() = 0;
	virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers) = 0;
    virtual void releaseDeviceDependentResources() = 0;
    
    virtual void render(int flags = 0) = 0;
};

#endif /* defined(__noctisgames__Renderer__) */
