//
//  Direct3DProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DProgram__
#define __noctisgames__Direct3DProgram__

#include "pch.h"

class Direct3DProgram
{
public:
    Direct3DProgram(_In_z_ const wchar_t* vertexShaderName, _In_z_ const wchar_t* pixelShaderName, bool useTextureCoords);
    
    virtual ~Direct3DProgram();
    
    virtual void mapVertices() = 0;
    
    void bindShaders();
    
    void bindMatrix();
    
    void bindNormalSamplerState();
    
    void bindWrapSamplerState();

	void createConstantBuffer(_COM_Outptr_opt_  ID3D11Buffer **ppBuffer);
    
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
};

#endif /* defined(__noctisgames__Direct3DProgram__) */
