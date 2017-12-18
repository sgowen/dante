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
    
    virtual void createDeviceDependentResources();
    
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers);
    
    virtual void releaseDeviceDependentResources();
    
    virtual void beginFrame();
    
    virtual void endFrame();
    
    virtual NGTexture* getFramebuffer(int index);
    
    virtual void updateMatrix(float left, float right, float bottom, float top);
    
    virtual void bindToOffscreenFramebuffer(int index);
    
    virtual void clearFramebufferWithColor(float r, float g, float b, float a);
    
    virtual void bindToScreenFramebuffer();
    
    virtual void useNormalBlending();
    virtual void useScreenBlending();
    
    virtual void bindMatrix(int32_t location = 0);
    
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, int32_t location = 0);
    virtual void destroyTexture(TextureWrapper& textureWrapper);
    
    virtual void bindShaderProgram(ShaderProgramWrapper* shaderProgramWrapper);
    virtual void destroyShaderProgram(ShaderProgramWrapper* shaderProgramWrapper);
    
    virtual void mapTextureVertices() = 0;
    virtual void unmapTextureVertices() = 0;
    
    virtual void mapColorVertices() = 0;
    virtual void unmapColorVertices() = 0;
    
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t count);
    
    virtual GLuint getTextureVertexBuffer();
    virtual GLuint getColorVertexBuffer();
    virtual mat4x4& getMatrix();
    
protected:
    virtual void createFramebufferObject();
    virtual void releaseFramebuffers();
    
private:
    std::vector<GLuint> _fbos;
    std::vector<GLuint> _fbo_textures;
    
    GLuint _textureVboObject; // For Sprite Batcher
    GLuint _colorVboObject; // For Geometry Batcher
    
    GLint _screenFBO;
    GLint _maxTextureSize;
    
    mat4x4 _matrix;
    
    int slotIndexForTextureSlot(NGTextureSlot textureSlot);
};

#endif /* defined(__noctisgames__OpenGLRendererHelper__) */
