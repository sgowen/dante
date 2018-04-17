//
//  RendererHelper.h
//  noctisgames
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
    
    virtual void clearFramebufferWithColor(float r, float g, float b, float a) = 0;
    virtual void useNormalBlending() = 0;
    virtual void useScreenBlending() = 0;
    virtual void disableBlending() = 0;
    virtual void bindInt4(NGShaderUniformInput* uniform, int4& inValue) = 0;
    virtual void bindFloat4(NGShaderUniformInput* uniform, float4& inValue) = 0;
    virtual void bindFloat4Array(NGShaderUniformInput* uniform, int count, float4* inValue) = 0;
    virtual void bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue) = 0;
    virtual void bindMatrix(NGShaderUniformInput* uniform);
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
    void bindFramebuffer(FramebufferWrapper* fbw);
    void bindScreenFramebuffer();
    FramebufferWrapper* addFramebuffer(std::string name, int renderWidth, int renderHeight, std::string textureFilterMin, std::string textureFilterMag);
    void removeFramebuffer(std::string name);
    void releaseFramebuffers();
    FramebufferWrapper* getOffscreenFramebuffer(int fbIndex);
    FramebufferWrapper* getFramebuffer(std::string name);
    
protected:
    FramebufferWrapper* _screenFramebufferWrapper;
    std::vector<FramebufferWrapper* > _offscreenFramebufferWrappers;
    std::map<std::string, FramebufferWrapper* > _framebufferWrappers;
    std::vector<GPUBufferWrapper* > _dynamicTextureVertexBuffers;
    std::vector<GPUBufferWrapper* > _staticTextureVertexBuffers;
    std::vector<GPUBufferWrapper* > _dynamicVertexBuffers;
    std::vector<GPUBufferWrapper* > _staticVertexBuffers;
    GPUBufferWrapper* _indexBuffer;
    GPUBufferWrapper* _staticScreenVertexBuffer;
    
    virtual void createScreenFramebufferWrapper(FramebufferWrapper* fbw) = 0;
    virtual TextureWrapper* createFramebufferImpl(FramebufferWrapper* fbw) = 0;
    virtual void destroyFramebufferImpl(FramebufferWrapper* fbw) = 0;
    virtual void bindFramebufferImpl(FramebufferWrapper* framebufferWrapper) = 0;
    virtual void bindViewport(int renderWidth, int renderHeight) = 0;
    virtual GPUBufferWrapper* createGPUBufferImpl(size_t size, const void *data, bool useStaticBuffer, bool isVertex) = 0;
    virtual void destroyGPUBufferImpl(GPUBufferWrapper* gpuBuffer) = 0;
    
private:
    mat4x4 _matrix;
    
    void createFramebuffer(FramebufferWrapper* fbw, std::string textureFilterMin, std::string textureFilterMag);
    void destroyFramebuffer(FramebufferWrapper* fbw);
    void createIndexBuffer();
    void createStaticScreenVertexBuffer();
    void destroyAllGPUBuffers(std::vector<GPUBufferWrapper* >& buffers);
    void releaseOffscreenFramebuffers();
};

#endif /* defined(__noctisgames__RendererHelper__) */
