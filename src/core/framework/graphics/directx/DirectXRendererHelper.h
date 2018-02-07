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
    virtual void mapTextureVertices(std::vector<VERTEX_2D_TEXTURE>& vertices);
    virtual void mapVertices(std::vector<VERTEX_2D>& vertices);
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    
protected:
    virtual TextureWrapper* createFramebuffer();
    virtual void platformReleaseFramebuffers();
    
private:
    // Cached pointer to device resources.
    static DX::DirectXDeviceResources* s_deviceResources;
    
    std::vector<ID3D11Texture2D*> _offscreenRenderTargets;
    std::vector<ID3D11RenderTargetView*> _offscreenRenderTargetViews;
    std::vector<ID3D11ShaderResourceView*> _offscreenShaderResourceViews;
    ID3D11Buffer* _textureVertexBuffer;
    ID3D11Buffer* _vertexBuffer;
    ID3D11Buffer* _indexbuffer;
    ID3D11BlendState* _blendState;
    ID3D11BlendState* _screenBlendState;
    ID3D11SamplerState* _textureSamplerState;
    ID3D11SamplerState* _textureWrapSamplerState;
    ID3D11SamplerState* _framebufferSamplerState;
	int _fbIndex;
    
    template <typename T>
    void mapVertices(ID3D11Buffer* vertexBuffer, std::vector<T>& vertices)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        
        // Disable GPU access to the vertex buffer data.
        getD3DContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        
        // Update the vertex buffer here.
        memcpy(mappedResource.pData, &vertices[0], sizeof(T) * vertices.size());
        
        // Reenable GPU access to the vertex buffer data.
        getD3DContext()->Unmap(vertexBuffer, 0);
        
        // Set the vertex buffer
        UINT stride = sizeof(T);
        UINT offset = 0;
        getD3DContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    }
    
    template <typename T>
    void createVertexBuffer(ID3D11Buffer** vertexBuffer, uint32_t size)
    {
        std::vector<T> vertices;
        vertices.reserve(size);
        
        T vertex;
        for (int i = 0; i < size; ++i)
        {
            vertices.push_back(vertex);
        }
        
        D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
        vertexBufferDesc.ByteWidth = sizeof(T) * vertices.size();
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
        vertexBufferData.pSysMem = &vertices[0];
        
        ID3D11Buffer* buffer;
        DX::ThrowIfFailed(getD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &buffer));
        *vertexBuffer = buffer;
    }
    
    void createIndexBuffer();
    void createBlendStates();
    void createSamplerStates();
    D3D11_FILTER filterForMinAndMag(std::string& cfgFilterMin, std::string& cfgFilterMag);
    void bindConstantBuffer(NGShaderUniformInput* uniform, const void *pSrcData);
};

#endif /* defined(__noctisgames__DirectXRendererHelper__) */
