//
//  RendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RendererHelper__
#define __noctisgames__RendererHelper__

#include <framework/graphics/portable/NGPrimitiveType.h>
#include <framework/graphics/portable/NGTextureSlot.h>
#include <framework/graphics/portable/VertexProgramInput.h>
#include <framework/graphics/portable/GPUBufferWrapper.h>

#include <vector>
#include <map>

class NGTexture;
struct TextureWrapper;
struct ShaderProgramWrapper;
class NGShaderUniformInput;
struct FramebufferWrapper;

class RendererHelper
{
public:
    RendererHelper();
    virtual ~RendererHelper();

    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight);
    virtual void releaseDeviceDependentResources();
    
    virtual void bindFramebuffer(FramebufferWrapper* framebufferWrapper) = 0;
    virtual void bindScreenFramebuffer() = 0;
    virtual void clearFramebufferWithColor(float r, float g, float b, float a) = 0;
    virtual void useNormalBlending() = 0;
    virtual void useScreenBlending() = 0;
    virtual void useNoBlending() = 0;
    virtual void bindInt4(NGShaderUniformInput* uniform, int4& inValue) = 0;
    virtual void bindFloat4(NGShaderUniformInput* uniform, float4& inValue) = 0;
    virtual void bindFloat4Array(NGShaderUniformInput* uniform, int count, float4* inValue) = 0;
    virtual void bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue) = 0;
    virtual void bindMatrix(NGShaderUniformInput* uniform) = 0;
    virtual void bindShader(ShaderProgramWrapper* shaderProgramWrapper) = 0;
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform = NULL) = 0;
    virtual void mapTextureVertices(std::vector<VERTEX_2D_TEXTURE>& vertices, bool useStaticBuffer = false, int gpuBufferIndex = 0) = 0;
    virtual void mapVertices(std::vector<VERTEX_2D>& vertices, bool useStaticBuffer = false, int gpuBufferIndex = 0) = 0;
    virtual void bindTextureVertexBuffer(bool useStaticBuffer = false, int gpuBufferIndex = 0) = 0;
    virtual void bindVertexBuffer(bool useStaticBuffer = false, int gpuBufferIndex = 0) = 0;
    virtual void bindScreenVertexBuffer() = 0;
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count) = 0;
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count) = 0;
    
    void updateMatrix(float left, float right, float bottom, float top);
    FramebufferWrapper* getOffscreenFramebuffer(int fbIndex);
    FramebufferWrapper* getFramebuffer(std::string name);
    FramebufferWrapper* addFramebuffer(int renderWidth, int renderHeight, std::string name);
    void removeFramebuffer(std::string name);
    void clearFramebuffers();
    
protected:
    std::vector<FramebufferWrapper* > _offscreenFramebufferWrappers;
    std::map<std::string, FramebufferWrapper* > _framebufferWrappers;
    std::vector<GPUBufferWrapper* > _dynamicTextureVertexBuffers;
    std::vector<GPUBufferWrapper* > _staticTextureVertexBuffers;
    std::vector<GPUBufferWrapper* > _dynamicVertexBuffers;
    std::vector<GPUBufferWrapper* > _staticVertexBuffers;
    GPUBufferWrapper* _indexBuffer;
    GPUBufferWrapper* _staticScreenVertexBuffer;
    mat4x4 _matrix;
    int _screenWidth;
    int _screenHeight;
    
    virtual GPUBufferWrapper* createGPUBuffer(size_t size, const void *data, bool useStaticBuffer, bool isVertex) = 0;
    virtual void destroyGPUBuffer(GPUBufferWrapper* gpuBuffer) = 0;
    virtual TextureWrapper* platformCreateFramebuffer(FramebufferWrapper* framebufferWrapper) = 0;
    virtual void destroyFramebuffer(FramebufferWrapper* fb) = 0;
    virtual void onFramebufferBinded(int renderWidth, int renderHeight) = 0;
    
private:
    void createFramebuffer(FramebufferWrapper* framebufferWrapper);
    void createIndexBuffer();
    void createStaticScreenVertexBuffer();
    void destroyAllGPUBuffers(std::vector<GPUBufferWrapper* >& buffers);
    void releaseOffscreenFramebuffers();
    void releaseFramebuffers();
};

#endif /* defined(__noctisgames__RendererHelper__) */
