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

class DirectXRendererHelper : public RendererHelper
{
public:
    static void init(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11RenderTargetView* d3dRenderTargetView);
    
    static ID3D11Device* getD3dDevice();
    
    static ID3D11DeviceContext* getD3dContext();
    
    DirectXRendererHelper();
    
    virtual ~DirectXRendererHelper();

	virtual void createDeviceDependentResources();

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
    
    virtual void bindMatrix(NGShaderUniformInput* uniform);
    
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform = NULL);
    virtual void destroyTexture(TextureWrapper& textureWrapper);
    
    virtual void bindShaderProgram(ShaderProgramWrapper* shaderProgramWrapper);
    virtual void destroyShaderProgram(ShaderProgramWrapper* shaderProgramWrapper, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout);
    
    virtual void mapScreenVertices(std::vector<NGShaderVarInput*>& inputLayout);
    virtual void unmapScreenVertices();
    
    virtual void mapTextureVertices(std::vector<NGShaderVarInput*>& inputLayout);
    virtual void unmapTextureVertices();
    
    virtual void mapColorVertices(std::vector<NGShaderVarInput*>& inputLayout);
    virtual void unmapColorVertices();
    
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t count);
    
protected:
    virtual void createFramebufferObject();
    virtual void releaseFramebuffers();
    
private:
    // Cached pointer to device resources.
    static ID3D11Device* s_d3dDevice;
    static ID3D11DeviceContext* s_d3dContext;
    static ID3D11RenderTargetView* s_d3dRenderTargetView;
    
    std::vector<ID3D11Texture2D*> _offscreenRenderTargets; // the offscreen render target texture
    std::vector<ID3D11RenderTargetView*> _offscreenRenderTargetViews; // the offscreen render target interface
    std::vector<ID3D11ShaderResourceView*> _offscreenShaderResourceViews; // this is needed for the screen pixel shader
    
    Microsoft::WRL::ComPtr<ID3D11BlendState> _blendState; // the blend state interface
    Microsoft::WRL::ComPtr<ID3D11BlendState> _screenBlendState; // the blend state interface, but for rendering to the screen
    Microsoft::WRL::ComPtr<ID3D11Buffer> _indexbuffer; // the index buffer interface
    
    // Used in SpriteBatcher
    Microsoft::WRL::ComPtr<ID3D11SamplerState> _sbSamplerState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> _sbWrapSamplerState;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _textureVertexBuffer;
    
    // Used in NGRectBatcher, LineBatcher, and CircleBatcher (Geometry)
    Microsoft::WRL::ComPtr<ID3D11Buffer> _colorVertexBuffer; // the vertex buffer interface
    
    // Used when rendering directly to the screen
    Microsoft::WRL::ComPtr<ID3D11Buffer> _screenVertexBuffer;
    
    // All above rendering takes place inside this matrix
    DirectX::XMFLOAT4X4 _matrix;
    
	int _framebufferIndex;
    
    template <typename T>
    void mapVertices(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, std::vector<T>& vertices)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        
        //    Disable GPU access to the vertex buffer data.
        s_d3dContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        
        //    Update the vertex buffer here.
        memcpy(mappedResource.pData, &vertices.front(), sizeof(T) * vertices.size());
        
        //    Reenable GPU access to the vertex buffer data.
        s_d3dContext->Unmap(vertexBuffer.Get(), 0);
        
        // Set the vertex buffer
        UINT stride = sizeof(T);
        UINT offset = 0;
        s_d3dContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    }
    
    void createBlendStates();
    void createSamplerStates();
    void createVertexBufferForSpriteBatcher();
    void createVertexBufferForGeometryBatchers();
	void createVertexBufferForScreen();
    void createIndexBuffer();
};

#endif /* defined(__noctisgames__DirectXRendererHelper__) */
