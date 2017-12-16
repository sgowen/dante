//
//  RendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RendererHelper__
#define __noctisgames__RendererHelper__

class TextureWrapper;
struct GpuTextureWrapper;

class RendererHelper
{
public:
    RendererHelper();
    
    virtual ~RendererHelper();

	virtual void createDeviceDependentResources(int maxBatchSize) = 0;

	virtual void createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers) = 0;

	virtual void releaseDeviceDependentResources() = 0;
    
    virtual void beginFrame() = 0;
    
    virtual void endFrame() = 0;
    
    virtual TextureWrapper* getFramebuffer(int index) = 0;
    
    virtual void updateMatrix(float left, float right, float bottom, float top) = 0;
    
    virtual void bindToOffscreenFramebuffer(int index) = 0;
    
    virtual void clearFramebufferWithColor(float r, float g, float b, float a) = 0;
    
    virtual void bindToScreenFramebuffer() = 0;
    
    virtual void destroyTexture(GpuTextureWrapper& textureWrapper) = 0;
    
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v) = 0;
    
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a) = 0;
    
protected:
    TextureWrapper* _framebuffer;
};

#endif /* defined(__noctisgames__RendererHelper__) */
