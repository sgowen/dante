//
//  OpenGLRendererHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLRendererHelper__
#define __noctisgames__OpenGLRendererHelper__

#include <framework/graphics/portable/RendererHelper.h>

class NGShaderVarInput;

class OpenGLRendererHelper : public RendererHelper
{
public:
    OpenGLRendererHelper();
    virtual ~OpenGLRendererHelper();
    
    virtual void clearFramebufferWithColor(float r, float g, float b, float a);
    virtual void useNormalBlending();
    virtual void useScreenBlending();
    virtual void disableBlending();
    virtual void bindInt4(NGShaderUniformInput* uniform, int4& inValue);
    virtual void bindFloat4(NGShaderUniformInput* uniform, float4& inValue);
    virtual void bindFloat4Array(NGShaderUniformInput* uniform, int count, float4* inValue);
    virtual void bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue);
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
    virtual void createScreenFramebufferWrapper(FramebufferWrapper* fbw);
    virtual TextureWrapper* createFramebufferImpl(FramebufferWrapper* fbw);
    virtual void destroyFramebufferImpl(FramebufferWrapper* fbw);
    virtual void bindFramebufferImpl(FramebufferWrapper* fbw);
    virtual void bindViewport(int renderWidth, int renderHeight);
    virtual GPUBufferWrapper* createGPUBufferImpl(size_t size, const void *data, bool useStaticBuffer, bool isVertex);
    virtual void destroyGPUBufferImpl(GPUBufferWrapper* gpuBuffer);

private:
    ShaderProgramWrapper* _currentShaderProgramWrapper;
    
    void bindInputLayout(std::vector<NGShaderVarInput*>& inputLayout);
};

#endif /* defined(__noctisgames__OpenGLRendererHelper__) */
