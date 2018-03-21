//
//  DirectXRendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXRendererHelper__
#define __noctisgames__DirectXRendererHelper__

#include <framework/graphics/portable/RendererHelper.h>

class DirectXRendererHelper : public RendererHelper
{
public:
    static void init(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext);
    static void init(ID3D11RenderTargetView* renderTargetView);
    static ID3D11Device* getD3DDevice();
    static ID3D11DeviceContext* getD3DContext();
    
    DirectXRendererHelper();
    virtual ~DirectXRendererHelper();

	virtual void createDeviceDependentResources();
	virtual void releaseDeviceDependentResources();
    
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
    // Cached pointer to device resources.
    static ID3D11Device* s_d3dDevice;
    static ID3D11DeviceContext* s_d3dContext;
    static ID3D11RenderTargetView* s_d3dRenderTargetView;
    
    ID3D11BlendState* _blendState;
    ID3D11BlendState* _screenBlendState;
    ID3D11SamplerState* _framebufferSamplerState; // mipmap: false, min: FramebufferFilterMin, mag: FramebufferFilterMag
    ID3D11SamplerState* _textureSamplerState1; // mipmap: true, min: GL_NEAREST, min: GL_NEAREST
    ID3D11SamplerState* _textureSamplerState2; // mipmap: true, min: GL_LINEAR,  min: GL_NEAREST
    ID3D11SamplerState* _textureSamplerState3; // mipmap: true, min: GL_LINEAR,  min: GL_LINEAR
    ID3D11SamplerState* _textureSamplerState4; // mipmap: true, min: GL_NEAREST,  min: GL_LINEAR
    ID3D11SamplerState* _textureSamplerState5; // mipmap: false, min: GL_NEAREST, min: GL_NEAREST
    ID3D11SamplerState* _textureSamplerState6; // mipmap: false, min: GL_LINEAR,  min: GL_NEAREST
    ID3D11SamplerState* _textureSamplerState7; // mipmap: false, min: GL_LINEAR,  min: GL_LINEAR
    ID3D11SamplerState* _textureSamplerState8; // mipmap: false, min: GL_NEAREST,  min: GL_LINEAR
    FramebufferWrapper* _currentlyBoundFramebufferWrapper;
    
    void createBlendStates();
    void createSamplerStates();
    D3D11_FILTER filterForMinAndMag(std::string cfgFilterMin, std::string cfgFilterMag, bool mipmap = false);
    ID3D11SamplerState* getSamplerStateForTexture(NGTexture* texture);
    void mapVertexBuffer(ID3D11Buffer* buffer, const void *data, size_t size, UINT stride);
    void bindVertexBuffer(ID3D11Buffer* buffer, UINT stride);
    void bindConstantBuffer(NGShaderUniformInput* uniform, const void *pSrcData);
};

#endif /* defined(__noctisgames__DirectXRendererHelper__) */
