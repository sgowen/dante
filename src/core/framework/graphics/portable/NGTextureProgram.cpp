//
//  NGTextureProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/portable/NGTextureProgram.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <assert.h>

NGTextureProgram::NGTextureProgram(RendererHelper& inRendererHelper, ShaderProgramLoader& inShaderProgramLoader, const char* vertexShaderName, const char* fragmentShaderName) : ShaderProgram(inRendererHelper, inShaderProgramLoader, vertexShaderName, fragmentShaderName)
{
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix"));
    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit"));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 3, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_Color", 4, 3));
    _inputLayout.push_back(new NGShaderVarInput("a_TexCoord", 2, 7));
    
    load();
}

void NGTextureProgram::bind(void* data)
{
    assert(data != NULL);
    
    _rendererHelper.bindShaderProgram(_shaderProgramWrapper);
    
    _rendererHelper.useNormalBlending();
    
    _rendererHelper.bindMatrix(_uniforms[0]);
    
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data), _uniforms[1]);
    
    _rendererHelper.mapTextureVertices(_inputLayout);
}

void NGTextureProgram::unbind()
{
    _rendererHelper.unmapTextureVertices();
    
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    
    _rendererHelper.bindShaderProgram(NULL);
}
