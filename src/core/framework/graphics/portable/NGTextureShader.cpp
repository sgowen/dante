//
//  NGTextureShader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGTextureShader.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/NGShaderLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>
#include <framework/graphics/portable/Color.h>

#include <assert.h>

NGTextureShader::NGTextureShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, vertexShaderName, fragmentShaderName)
{
    // Vertex Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix", 0, 64, false));
    
    // Fragment Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Color", 0, 16, true));
    
    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit", 1));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_TexCoord", 2, 2));
}

void NGTextureShader::bind(void* vertices, void* data1, void* data2, void* data3)
{
    assert(vertices != NULL);
    assert(data1 != NULL);
    assert(data3 != NULL);
    
    _rendererHelper.bindNGShader(_shaderProgramWrapper);
    _rendererHelper.bindMatrix(_uniforms[0]);
    Color* color = static_cast<Color* >(data3);
    float4 float4Color;
    float4Color[0] = color->red;
    float4Color[1] = color->green;
    float4Color[2] = color->blue;
    float4Color[3] = color->alpha;
    _rendererHelper.bindFloat4(_uniforms[1], float4Color);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[2]);
    
    std::vector<VERTEX_2D_TEXTURE>* textureVertices = static_cast<std::vector<VERTEX_2D_TEXTURE>* >(vertices);
    _rendererHelper.mapTextureVertices(_inputLayout, *textureVertices);
}

void NGTextureShader::unbind()
{
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    _rendererHelper.bindNGShader(NULL);
}
