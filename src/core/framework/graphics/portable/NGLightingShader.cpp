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

NGLightingShader::NGLightingShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, vertexShaderName, fragmentShaderName), _lightIndex(0)
{
    // Vertex Shader
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix",          0,  64, false));
    _uniforms.push_back(new NGShaderUniformInput("u_LightPositions",  1, 192, false));
    _uniforms.push_back(new NGShaderUniformInput("u_NumLights",       2,  16, false));
    
    // Fragment Shader
    _uniforms.push_back(new NGShaderUniformInput("u_LightColors",     0, 192, true));
    _uniforms.push_back(new NGShaderUniformInput("u_AmbientColor",    1,  16, true));
    _uniforms.push_back(new NGShaderUniformInput("u_Falloff",         2,  16, true));
    _uniforms.push_back(new NGShaderUniformInput("u_NumLights",       3,  16, true));

    // Textures
    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit",     0));
    _uniforms.push_back(new NGShaderUniformInput("u_NormalMapUnit",   1));
    
    // Vertices
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_TexCoord", 2, 2));
    
    resetLights();
}

void NGLightingShader::bind(void* input, void* data1, void* data2, void* data3)
{
    assert(input != NULL);
    assert(data1 != NULL);
    assert(data2 != NULL);
    
    _rendererHelper.bindShader(_shaderProgramWrapper);
    _rendererHelper.bindMatrix(                                                     _uniforms[0]);
    _rendererHelper.bindFloat4Array(                                                _uniforms[1], _numLights[0], _lightPositions);
    _rendererHelper.bindInt4(                                                       _uniforms[2], _numLights);
    _rendererHelper.bindFloat4Array(                                                _uniforms[3], _numLights[0], _lightColors);
    _rendererHelper.bindFloat4(                                                     _uniforms[4], _ambientColor);
    _rendererHelper.bindFloat4(                                                     _uniforms[5], _fallOff);
    _rendererHelper.bindInt4(                                                       _uniforms[6], _numLights);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[7]);
    _rendererHelper.bindTexture(NGTextureSlot_ONE, static_cast<NGTexture*>(data2),  _uniforms[8]);
    
    std::vector<VERTEX_2D_TEXTURE>* vertices = static_cast<std::vector<VERTEX_2D_TEXTURE>* >(input);
    _rendererHelper.mapTextureVertices(*vertices);
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
