//
//  DirectXProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXProgram__
#define __noctisgames__DirectXProgram__

#include <framework/graphics/portable/GpuProgram.h>

#include "pch.h"

class DirectXRendererHelper;

class DirectXProgram : public GpuProgram
{
public:
    DirectXProgram(DirectXRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName, bool useTextureCoords);
    
    virtual ~DirectXProgram();
    
    virtual void bind();
    
    virtual void unbind();
    
protected:
	DirectXRendererHelper* _rendererHelper;
    
    void bindMatrix();
    
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;
    
    void createConstantBuffer(_COM_Outptr_opt_  ID3D11Buffer **ppBuffer);
};

#endif /* defined(__noctisgames__DirectXProgram__) */
