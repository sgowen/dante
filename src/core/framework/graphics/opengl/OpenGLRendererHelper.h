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

typedef float vec4[4];
typedef vec4 mat4x4[4];

class OpenGLRendererHelper : public RendererHelper
{
public:
    OpenGLRendererHelper();
    
    virtual ~OpenGLRendererHelper();
    
    virtual void createDeviceDependentResources();
    
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers);
    
    virtual void releaseDeviceDependentResources();
    
    virtual void beginFrame();
    
    virtual void endFrame();
    
    virtual TextureWrapper* getFramebuffer(int index);
    
    virtual void updateMatrix(float left, float right, float bottom, float top);
    
    virtual void bindToOffscreenFramebuffer(int index);
    
    virtual void clearFramebufferWithColor(float r, float g, float b, float a);
    
    virtual void bindToScreenFramebuffer();
    
    virtual void destroyTexture(GpuTextureWrapper& textureWrapper);
    
    virtual void useNormalBlending();
    
    virtual void useScreenBlending();
    
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t count);
    
    virtual void bindTexture(NGTextureSlot textureSlot, TextureWrapper* textureWrapper, int32_t flags = 0);
    
    GLuint& getSbVboObject(); // For Sprite Batcher
    GLuint& getGbVboObject(); // For Geometry Batcher
    mat4x4& getViewProjectionMatrix();
    
protected:
    virtual void createFramebufferObject();
    virtual void releaseFramebuffers();
    
private:
    std::vector<GLuint> _fbos;
    std::vector<GLuint> _fbo_textures;
    
    GLuint _sbVboObject; // For Sprite Batcher
    GLuint _gbVboObject; // For Geometry Batcher
    
    GLint _screenFBO;
    GLint _maxTextureSize;
    
    mat4x4 _viewProjectionMatrix;
    
    int slotIndexForTextureSlot(NGTextureSlot textureSlot);
};

#endif /* defined(__noctisgames__OpenGLRendererHelper__) */
