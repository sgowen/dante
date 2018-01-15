//
//  NGLightingShader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGLightingShader.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/NGShaderLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <assert.h>

float NGLightingShader::DEFAULT_LIGHT_Z = 0.075f;
Vector3 NGLightingShader::LIGHT_POS(0.0f, 0.0f, DEFAULT_LIGHT_Z);
Color NGLightingShader::LIGHT_COLOR(1.0f, 0.8f, 0.6f, 1.0f);
Color NGLightingShader::AMBIENT_COLOR(0.6f, 0.6f, 1.0f, 0.4f);
Vector3 NGLightingShader::FALLOFF(0.8f, 3.0f, 20.0f);

NGLightingShader::NGLightingShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, vertexShaderName, fragmentShaderName)
{
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix", 64));
    _uniforms.push_back(new NGShaderUniformInput("u_LightPos", 12));
    _uniforms.push_back(new NGShaderUniformInput("u_LightColor", 16));
    _uniforms.push_back(new NGShaderUniformInput("u_AmbientColor", 16));
    _uniforms.push_back(new NGShaderUniformInput("u_Falloff", 12));

    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit"));
    _uniforms.push_back(new NGShaderUniformInput("u_NormalMapUnit"));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_Color", 4, 2));
    _inputLayout.push_back(new NGShaderVarInput("a_TexCoord", 2, 6));
}

void NGLightingShader::bind(void* vertices, void* data1, void* data2)
{
    assert(vertices != NULL);
    assert(data1 != NULL);
    assert(data2 != NULL);
    
    _rendererHelper.bindNGShader(_shaderProgramWrapper);
    _rendererHelper.bindMatrix(_uniforms[0]);
    _rendererHelper.bindVector3(_uniforms[1], _lightPos);
    _rendererHelper.bindColor(_uniforms[2], LIGHT_COLOR);
    _rendererHelper.bindColor(_uniforms[3], AMBIENT_COLOR);
    _rendererHelper.bindVector3(_uniforms[4], FALLOFF);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[5]);
    _rendererHelper.bindTexture(NGTextureSlot_ONE, static_cast<NGTexture*>(data2), _uniforms[6]);
    
    std::vector<TEXTURE_VERTEX>* textureVertices = static_cast<std::vector<TEXTURE_VERTEX>* >(vertices);
    _rendererHelper.mapTextureVertices(_inputLayout, *textureVertices);
}

void NGLightingShader::unbind()
{
    _rendererHelper.unmapScreenVertices();
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    _rendererHelper.bindTexture(NGTextureSlot_ONE, NULL);
    _rendererHelper.bindNGShader(NULL);
}

void NGLightingShader::config(float lightPosX, float lightPosY, float lightPosZ)
{
    _lightPos.set(lightPosX, lightPosY, lightPosZ);
}
