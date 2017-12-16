//
//  OpenGLRendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLRendererHelper__
#define __noctisgames__OpenGLRendererHelper__

#include "framework/graphics/portable/RendererHelper.h"

class OpenGLRendererHelper : public RendererHelper
{
public:
    OpenGLRendererHelper();
    
    virtual ~OpenGLRendererHelper();
    
    virtual void createDeviceDependentResources(int maxBatchSize);
    
    virtual void createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers);
    
    virtual void releaseDeviceDependentResources();
    
    virtual void beginFrame();
    
    virtual void endFrame();
    
    virtual TextureWrapper* getFramebuffer(int index);
    
    virtual void updateMatrix(float left, float right, float bottom, float top);
    
    virtual void bindToOffscreenFramebuffer(int index);
    
    virtual void clearFramebufferWithColor(float r, float g, float b, float a);
    
    virtual void bindToScreenFramebuffer();
    
    virtual void destroyTexture(GpuTextureWrapper& textureWrapper);
    
    virtual void clearColorVertices();
    
    virtual void clearTextureVertices();
    
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v);
    
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a);
    
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t count);
    
    virtual void bindTexture(NGTextureSlot textureSlot, TextureWrapper* textureWrapper);
};

#endif /* defined(__noctisgames__OpenGLRendererHelper__) */
