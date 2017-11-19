//
//  DirectXManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/17/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXManager__
#define __noctisgames__DirectXManager__

#define VERTICES_PER_LINE 2
#define VERTICES_PER_RECTANGLE 4
#define INDICES_PER_RECTANGLE 6

#define DXManager (DirectXManager::getInstance())

#include "framework/graphics/directx/DirectXProgramInput.h"
#include "pch.h"

#include <vector>

struct GpuTextureWrapper;

class DirectXManager
{
public:
	static void init(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11RenderTargetView* d3dRenderTargetView, DirectX::XMFLOAT4X4 orientation);

	static ID3D11Device* getD3dDevice();

	static ID3D11DeviceContext* getD3dContext();

	static ID3D11RenderTargetView* getD3dRenderTargetView();

    static void create();
    
    static void destroy();
    
    static DirectXManager * getInstance();
    
    void createDeviceDependentResources(int maxBatchSize);
    
    void createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers);
    
    void releaseDeviceDependentResources();

	void updateMatrix(float left, float right, float bottom, float top);

	// Called by Batchers
	void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v); 
	void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a);
    
    void useNormalBlending();
    void useScreenBlending();
    
	std::vector<ID3D11Texture2D*>& getOffscreenRenderTargets();
	std::vector<ID3D11RenderTargetView*>& getOffscreenRenderTargetViews();
	std::vector<ID3D11ShaderResourceView*>& getOffscreenShaderResourceViews();
	std::vector<GpuTextureWrapper *>& getFramebuffers();
	Microsoft::WRL::ComPtr<ID3D11BlendState>& getBlendState();
	Microsoft::WRL::ComPtr<ID3D11BlendState>& getScreenBlendState();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& getMatrixConstantbuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& getIndexbuffer();
	Microsoft::WRL::ComPtr<ID3D11SamplerState>& getSbSamplerState();
	Microsoft::WRL::ComPtr<ID3D11SamplerState>& getSbWrapSamplerState();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& getSbVertexBuffer();
	std::vector<TEXTURE_VERTEX>& getTextureVertices();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& getGbVertexBuffer();
	std::vector<COLOR_VERTEX>& getColorVertices();
	DirectX::XMFLOAT4X4& getMatFinal();
    bool isWindowsMobile();

private:
    static DirectXManager* s_pInstance;
    
	// Cached pointer to device resources.
	static ID3D11Device* s_d3dDevice;
	static ID3D11DeviceContext* s_d3dContext;
	static ID3D11RenderTargetView* s_d3dRenderTargetView;
	static DirectX::XMFLOAT4X4 s_orientation;

	std::vector<ID3D11Texture2D*> _offscreenRenderTargets; // the offscreen render target texture
	std::vector<ID3D11RenderTargetView*> _offscreenRenderTargetViews; // the offscreen render target interface
	std::vector<ID3D11ShaderResourceView*> _offscreenShaderResourceViews; // this is needed for the screen pixel shader
    std::vector<GpuTextureWrapper *> _framebuffers;
	Microsoft::WRL::ComPtr<ID3D11BlendState> _blendState; // the blend state interface
	Microsoft::WRL::ComPtr<ID3D11BlendState> _screenBlendState; // the blend state interface, but for rendering to the screen
	Microsoft::WRL::ComPtr<ID3D11Buffer> _matrixConstantbuffer; // the matrix constant buffer interface
	Microsoft::WRL::ComPtr<ID3D11Buffer> _indexbuffer; // the index buffer interface

	// Used in SpriteBatcher
	Microsoft::WRL::ComPtr<ID3D11SamplerState> _sbSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> _sbWrapSamplerState;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _sbVertexBuffer;
	std::vector<TEXTURE_VERTEX> _textureVertices;

	// Used in NGRectBatcher, LineBatcher, and CircleBatcher (Geometry)
	Microsoft::WRL::ComPtr<ID3D11Buffer> _gbVertexBuffer; // the vertex buffer interface
	std::vector<COLOR_VERTEX> _colorVertices;

	// All above rendering takes place inside this matrix
	DirectX::XMFLOAT4X4 _matFinal;
    
    int _renderWidth;
    int _renderHeight;
	int _numFramebuffers;
    
    bool _isWindowsMobile;

	void createBlendStates();
	void createSamplerStates();
	void createVertexBufferForSpriteBatcher(int maxBatchSize);
	void createVertexBufferForGeometryBatchers(int maxBatchSize);
	void createIndexBuffer(int maxBatchSize);
	void createConstantBuffer();

	std::vector<short> createIndexValues(int maxBatchSize);
    void createFramebufferObjects();
    void createFramebufferObject();
    void releaseFramebuffers();

	// ctor, copy ctor, and assignment should be private in a Singleton
	DirectXManager();
    ~DirectXManager();
	DirectXManager(const DirectXManager&);
	DirectXManager& operator=(const DirectXManager&);
};

#endif /* defined(__noctisgames__DirectXManager__) */
