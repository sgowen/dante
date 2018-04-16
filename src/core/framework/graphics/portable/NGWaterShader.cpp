//
//  NGWaterShader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/21/18.
//  Copyright (c) 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/NGWaterShader.h>

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>
#include <framework/graphics/portable/Color.h>
#include <framework/util/MathUtil.h>

#include <assert.h>

NGWaterShader::NGWaterShader(RendererHelper& inRendererHelper) : NGShader(inRendererHelper, "shader_003_vert.ngs", "shader_005_frag.ngs"),
_lastTime(0)
{
    // Vertex Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix", 0, 64, false));
    
    // Fragment Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Color", 0, 16, true));
    _uniforms.push_back(new NGShaderUniformInput("u_Time", 1, 16, true));
    
    // Textures
    _uniforms.push_back(new NGShaderUniformInput("u_Texture", 3));
    _uniforms.push_back(new NGShaderUniformInput("u_Texture2", 4));
    
    // Vertices
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_TexCoord", 2, 2));
    
    _time[0] = 0;
    _time[1] = 0;
    _time[2] = 0;
    _time[3] = 0;
}

void NGWaterShader::bind(void* data1, void* data2, void* data3)
{
    assert(data1 != NULL);
    assert(data2 != NULL);
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
    _rendererHelper.bindFloat4(_uniforms[2], _time);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[3]);
    _rendererHelper.bindTexture(NGTextureSlot_ONE, static_cast<NGTexture*>(data2),  _uniforms[4]);
}

void NGWaterShader::unbind()
{
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    _rendererHelper.bindTexture(NGTextureSlot_ONE, NULL);
    _rendererHelper.bindShader(NULL);
}

void NGWaterShader::update(float deltaTime)
{
    _lastTime += deltaTime;
    float angle = _lastTime * (2 * NG_M_PI);
    if (angle > (2 * NG_M_PI))
    {
        angle -= (2 * NG_M_PI);
    }
    
    _time[0] = -angle;
}
