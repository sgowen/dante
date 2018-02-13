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

class NGTexture;
struct TextureWrapper;
struct ShaderProgramWrapper;
class NGShaderUniformInput;

class RendererHelper
{
public:
    RendererHelper();
    virtual ~RendererHelper();

    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight);
    virtual void releaseDeviceDependentResources();
    
    virtual void bindToOffscreenFramebuffer(int index) = 0;
    virtual void clearFramebufferWithColor(float r, float g, float b, float a) = 0;
    virtual void bindToScreenFramebuffer() = 0;
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
    NGTexture* getFramebuffer(int index);
    
protected:
    std::vector<NGTexture *> _framebufferWrappers;
    std::vector<GPUBufferWrapper* > _dynamicTextureVertexBuffers;
    std::vector<GPUBufferWrapper* > _staticTextureVertexBuffers;
    std::vector<GPUBufferWrapper* > _dynamicVertexBuffers;
    std::vector<GPUBufferWrapper* > _staticVertexBuffers;
    GPUBufferWrapper* _indexBuffer;
    GPUBufferWrapper* _staticScreenVertexBuffer;
    mat4x4 _matrix;
    int _screenWidth;
    int _screenHeight;
    int _renderWidth;
    int _renderHeight;
    
    virtual GPUBufferWrapper* createGPUBuffer(size_t size, const void *data, bool useStaticBuffer, bool isVertex) = 0;
    virtual void disposeGPUBuffer(GPUBufferWrapper* gpuBuffer) = 0;
    virtual TextureWrapper* createFramebuffer() = 0;
    virtual void platformReleaseFramebuffers() = 0;
    
private:
    void createIndexBuffer();
    void createStaticScreenVertexBuffer();
    void disposeAllGPUBuffers(std::vector<GPUBufferWrapper* >& buffers);
    void releaseFramebuffers();
};

#endif /* defined(__noctisgames__RendererHelper__) */
