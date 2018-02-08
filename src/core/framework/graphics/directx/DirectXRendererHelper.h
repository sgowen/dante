//
//  DirectXRendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXRendererHelper__
#define __noctisgames__DirectXRendererHelper__

#include "framework/graphics/portable/RendererHelper.h"

#include "framework/main/directx/DirectXDeviceResources.h"

class DirectXRendererHelper : public RendererHelper
{
public:
    static void init(DX::DirectXDeviceResources* deviceResources);
    static ID3D11Device* getD3DDevice();
    static ID3D11DeviceContext* getD3DContext();
    
    DirectXRendererHelper();
    virtual ~DirectXRendererHelper();

	virtual void createDeviceDependentResources();
	virtual void releaseDeviceDependentResources();
    virtual void bindToOffscreenFramebuffer(int index);
    virtual void clearFramebufferWithColor(float r, float g, float b, float a);
    virtual void bindToScreenFramebuffer();
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
    virtual void mapTextureVertices(std::vector<VERTEX_2D_TEXTURE>& vertices, bool isDynamic = true, int gpuBufferIndex = 0);
    virtual void mapVertices(std::vector<VERTEX_2D>& vertices, bool isDynamic = true, int gpuBufferIndex = 0);
    virtual void bindScreenVertexBuffer();
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    
protected:
    virtual GPUBufferWrapper* createGPUBuffer(size_t size, const void *data, bool isDynamic, bool isVertex);
    virtual void disposeGPUBuffer(GPUBufferWrapper* gpuBuffer);
    virtual TextureWrapper* createFramebuffer();
    virtual void platformReleaseFramebuffers();
    
private:
    // Cached pointer to device resources.
    static DX::DirectXDeviceResources* s_deviceResources;
    
    std::vector<ID3D11Texture2D*> _offscreenRenderTargets;
    std::vector<ID3D11RenderTargetView*> _offscreenRenderTargetViews;
    std::vector<ID3D11ShaderResourceView*> _offscreenShaderResourceViews;
    ID3D11BlendState* _blendState;
    ID3D11BlendState* _screenBlendState;
    ID3D11SamplerState* _textureSamplerState;
    ID3D11SamplerState* _textureWrapSamplerState;
    ID3D11SamplerState* _framebufferSamplerState;
	int _fbIndex;
    
    void createBlendStates();
    void createSamplerStates();
    D3D11_FILTER filterForMinAndMag(std::string& cfgFilterMin, std::string& cfgFilterMag);
    void bindVertexBuffer(ID3D11Buffer* buffer, const void *data, size_t size);
    void bindConstantBuffer(NGShaderUniformInput* uniform, const void *pSrcData);
};

#endif /* defined(__noctisgames__DirectXRendererHelper__) */
