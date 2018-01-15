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
#include <framework/util/Config.h>

#include <assert.h>

NGLightingShader::NGLightingShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, vertexShaderName, fragmentShaderName),
_resolution(1440.0f, 900.0f),
_defaultLightZ(NG_CFG->getFloat("defaultLightZ")),
_lightPos(0, 0, _defaultLightZ),
_lightColor(NG_CFG->getFloat("LightColorR"), NG_CFG->getFloat("LightColorG"), NG_CFG->getFloat("LightColorB"), NG_CFG->getFloat("LightColorA")),
_ambientColor(NG_CFG->getFloat("AmbientColorR"), NG_CFG->getFloat("AmbientColorG"), NG_CFG->getFloat("AmbientColorB"), NG_CFG->getFloat("AmbientColorA")),
_fallOff(NG_CFG->getFloat("LightFalloffX"), NG_CFG->getFloat("LightFalloffY"), NG_CFG->getFloat("LightFalloffZ"))
{
    // Vertex Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix", 64));
    _uniforms.push_back(new NGShaderUniformInput("u_LightPos", 12));
    
    // Fragment Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Resolution", 8));
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
    _rendererHelper.bindVector2(_uniforms[2], _resolution);
    _rendererHelper.bindColor(_uniforms[3], _lightColor);
    _rendererHelper.bindColor(_uniforms[4], _ambientColor);
    _rendererHelper.bindVector3(_uniforms[5], _fallOff);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[6]);
    _rendererHelper.bindTexture(NGTextureSlot_ONE, static_cast<NGTexture*>(data2), _uniforms[7]);
    
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

void NGLightingShader::config(float resolutionX, float resolutionY, float lightPosX, float lightPosY)
{
    _resolution.set(resolutionX, resolutionY);
    
    _lightPos.set(lightPosX, lightPosY, _defaultLightZ);
}

void NGLightingShader::configZ(float lightPosZ)
{
    _lightPos.setZ(lightPosZ);
}
