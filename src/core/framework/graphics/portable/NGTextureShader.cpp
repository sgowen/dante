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

#include <assert.h>

NGTextureShader::NGTextureShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, vertexShaderName, fragmentShaderName)
{
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix", 64));
    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit"));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_Color", 4, 2));
    _inputLayout.push_back(new NGShaderVarInput("a_TexCoord", 2, 6));
}

void NGTextureShader::bind(void* vertices, void* data1, void* data2)
{
    assert(vertices != NULL);
    assert(data1 != NULL);
    
    _rendererHelper.bindNGShader(_shaderProgramWrapper);
    _rendererHelper.bindMatrix(_uniforms[0]);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[1]);
    
    std::vector<TEXTURE_VERTEX>* textureVertices = static_cast<std::vector<TEXTURE_VERTEX>* >(vertices);
    _rendererHelper.mapTextureVertices(_inputLayout, *textureVertices);
}

void NGTextureShader::unbind()
{
    _rendererHelper.unmapTextureVertices();
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    _rendererHelper.bindNGShader(NULL);
}
