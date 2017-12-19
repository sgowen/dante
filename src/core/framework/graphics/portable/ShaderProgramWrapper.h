//
//  ShaderProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ShaderProgramWrapper__
#define __noctisgames__ShaderProgramWrapper__

#include "framework/graphics/portable/NGGraphics.h"

struct ShaderProgramWrapper
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLuint _programObjectId;

    ShaderProgramWrapper(GLuint programObjectId) : _programObjectId(programObjectId) {}
#elif defined _WIN32
    Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;

    ShaderProgramWrapper(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout, Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader) : _vertexShader(vertexShader), _inputLayout(inputLayout), _pixelShader(pixelShader) {}
#endif
};

#endif /* defined(__noctisgames__ShaderProgramWrapper__) */
