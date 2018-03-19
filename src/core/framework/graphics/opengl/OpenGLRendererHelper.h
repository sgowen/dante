//
//  OpenGLRendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLRendererHelper__
#define __noctisgames__OpenGLRendererHelper__

#include <framework/graphics/portable/RendererHelper.h>

#include <framework/graphics/portable/ShaderProgramWrapper.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/util/Constants.h>

#include <framework/graphics/portable/NGShaderVarInput.h>

class OpenGLRendererHelper : public RendererHelper
{
public:
    OpenGLRendererHelper();
    virtual ~OpenGLRendererHelper();

    virtual void createDeviceDependentResources();
    
    virtual void bindToOffscreenFramebuffer(int index);
    virtual void bindToFramebuffer(int index);
    virtual void bindToScreenFramebuffer();
    virtual void clearFramebufferWithColor(float r, float g, float b, float a);
    virtual void useNormalBlending();
    virtual void useScreenBlending();
    virtual void useNoBlending();
    virtual void bindInt4(NGShaderUniformInput* uniform, int4& inValue);
    virtual void bindFloat4(NGShaderUniformInput* uniform, float4& inValue);
    virtual void bindFloat4Array(NGShaderUniformInput* uniform, int count, float4* inValue);
    virtual void bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue);
    virtual void bindMatrix(NGShaderUniformInput* uniform);
    virtual void bindShader(ShaderProgramWrapper* shaderProgramWrapper);
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform = NULL);
    virtual void mapTextureVertices(std::vector<VERTEX_2D_TEXTURE>& vertices, bool useStaticBuffer = false, int gpuBufferIndex = 0);
    virtual void mapVertices(std::vector<VERTEX_2D>& vertices, bool useStaticBuffer = false, int gpuBufferIndex = 0);
    virtual void bindTextureVertexBuffer(bool useStaticBuffer = false, int gpuBufferIndex = 0);
    virtual void bindVertexBuffer(bool useStaticBuffer = false, int gpuBufferIndex = 0);
    virtual void bindScreenVertexBuffer();
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);

protected:
    virtual GPUBufferWrapper* createGPUBuffer(size_t size, const void *data, bool useStaticBuffer, bool isVertex);
    virtual void disposeGPUBuffer(GPUBufferWrapper* gpuBuffer);
    virtual TextureWrapper* createOffscreenFramebuffer(int renderWidth, int renderHeight);
    virtual TextureWrapper* createFramebuffer(int renderWidth, int renderHeight);
    virtual void platformReleaseOffscreenFramebuffers();
    virtual void platformReleaseFramebuffers();

private:
    GLint _screenFBO;
    std::vector<GLuint> _offscreenFBOTextures;
    std::vector<GLuint> _offscreenFBOs;
    std::vector<GLuint> _FBOTextures;
    std::vector<GLuint> _FBOs;
    ShaderProgramWrapper* _currentShaderProgramWrapper;
    
    void createFramebuffer(GLuint& fbo_texture, GLuint& fbo, int renderWidth, int renderHeight);
    void bindToFramebuffer(FramebufferDef& framebufferDef);
    void bindInputLayout(std::vector<NGShaderVarInput*>& inputLayout);
};

#endif /* defined(__noctisgames__OpenGLRendererHelper__) */
