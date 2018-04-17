//
//  NGTextureShader.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/NGTextureShader.h>

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>
#include <framework/graphics/portable/Color.h>

#include <assert.h>

NGTextureShader::NGTextureShader(RendererHelper& inRendererHelper) : NGShader(inRendererHelper, "shader_003_vert.ngs", "shader_003_frag.ngs")
{
    // Vertex Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix", 0, 64, false));
    
    // Fragment Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Color", 0, 16, true));
    
    // Textures
    _uniforms.push_back(new NGShaderUniformInput("u_Texture", 2));
    
    // Vertices
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_TexCoord", 2, 2));
}

void NGTextureShader::bind(void* data1, void* data2, void* data3)
{
    assert(data1 != NULL);
    assert(data3 != NULL);
    
    _rendererHelper.bindShader(_shaderProgramWrapper);
    
    _rendererHelper.bindMatrix(_uniforms[0]);
    Color* color = static_cast<Color* >(data3);
    float4 float4Color;
    float4Color[0] = color->red;
    float4Color[1] = color->green;
    float4Color[2] = color->blue;
    float4Color[3] = color->alpha;
    _rendererHelper.bindFloat4(_uniforms[1], float4Color);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[2]);
}

void NGTextureShader::unbind()
{
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    _rendererHelper.bindShader(NULL);
}
