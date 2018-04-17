//
//  NGLightingShader.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/14/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/NGLightingShader.h>

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <assert.h>

NGLightingShader::NGLightingShader(RendererHelper& inRendererHelper) : NGShader(inRendererHelper, "shader_004_vert.ngs", "shader_004_frag.ngs"), _lightIndex(0)
{
    // Fragment Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix",          0,  64, true));
    _uniforms.push_back(new NGShaderUniformInput("u_LightPositions",  1, 192, true));
    _uniforms.push_back(new NGShaderUniformInput("u_LightColors",     2, 192, true));
    _uniforms.push_back(new NGShaderUniformInput("u_AmbientColor",    3,  16, true));
    _uniforms.push_back(new NGShaderUniformInput("u_Falloff",         4,  16, true));
    _uniforms.push_back(new NGShaderUniformInput("u_NumLights",       5,  16, true));

    // Textures
    _uniforms.push_back(new NGShaderUniformInput("u_Texture",         6));
    _uniforms.push_back(new NGShaderUniformInput("u_NormalMap",       7));
    
    // Vertices
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    
    resetLights();
}

void NGLightingShader::bind(void* data1, void* data2, void* data3)
{
    assert(data1 != NULL);
    assert(data2 != NULL);
    
    _rendererHelper.bindShader(_shaderProgramWrapper);
    _rendererHelper.bindMatrix(                                                     _uniforms[0]);
    _rendererHelper.bindFloat4Array(                                                _uniforms[1], _numLights[0], _lightPositions);
    _rendererHelper.bindFloat4Array(                                                _uniforms[2], _numLights[0], _lightColors);
    _rendererHelper.bindFloat4(                                                     _uniforms[3], _ambientColor);
    _rendererHelper.bindFloat4(                                                     _uniforms[4], _fallOff);
    _rendererHelper.bindInt4(                                                       _uniforms[5], _numLights);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[6]);
    _rendererHelper.bindTexture(NGTextureSlot_ONE, static_cast<NGTexture*>(data2),  _uniforms[7]);
}

void NGLightingShader::unbind()
{
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    _rendererHelper.bindTexture(NGTextureSlot_ONE, NULL);
    _rendererHelper.bindShader(NULL);
}

void NGLightingShader::resetLights()
{
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        _lightPositions[i][0] = 0;
        _lightPositions[i][1] = 0;
        _lightPositions[i][2] = 0;
        _lightPositions[i][3] = 0;
        
        _lightColors[i][0] = 0;
        _lightColors[i][1] = 0;
        _lightColors[i][2] = 0;
        _lightColors[i][3] = 0;
    }
    
    _ambientColor[0] = 0;
    _ambientColor[1] = 0;
    _ambientColor[2] = 0;
    _ambientColor[3] = 0;
    
    _fallOff[0] = 0;
    _fallOff[1] = 0;
    _fallOff[2] = 0;
    _fallOff[3] = 0;
    
    _numLights[0] = 0;
    _numLights[1] = 0;
    _numLights[2] = 0;
    _numLights[3] = 0;
    
    _lightIndex = 0;
}

void NGLightingShader::configAmbientLight(float r, float g, float b, float a)
{
    _ambientColor[0] = r;
    _ambientColor[1] = g;
    _ambientColor[2] = b;
    _ambientColor[3] = a;
}

void NGLightingShader::configureFallOff(float x, float y, float z)
{
    _fallOff[0] = x;
    _fallOff[1] = y;
    _fallOff[2] = z;
}

void NGLightingShader::addLight(float lightPosX, float lightPosY, float lightPosZ, float lightColorR, float lightColorG, float lightColorB, float lightColorA)
{
    _lightPositions[_lightIndex][0] = lightPosX;
    _lightPositions[_lightIndex][1] = lightPosY;
    _lightPositions[_lightIndex][2] = lightPosZ;
    
    _lightColors[_lightIndex][0] = lightColorR;
    _lightColors[_lightIndex][1] = lightColorG;
    _lightColors[_lightIndex][2] = lightColorB;
    _lightColors[_lightIndex][3] = lightColorA;
    
    _numLights[0] = ++_lightIndex;
}
