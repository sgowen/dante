//
//  NGFramebufferToScreenProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/portable/NGFramebufferToScreenProgram.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <assert.h>

NGFramebufferToScreenProgram::NGFramebufferToScreenProgram(RendererHelper& inRendererHelper, ShaderProgramLoader& inShaderProgramLoader, const char* vertexShaderName, const char* fragmentShaderName) : ShaderProgram(inRendererHelper, inShaderProgramLoader, vertexShaderName, fragmentShaderName)
{
    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit"));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 3, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_Color", 4, 3));
    _inputLayout.push_back(new NGShaderVarInput("a_TexCoord", 2, 7));
    
    load();
}

void NGFramebufferToScreenProgram::bind(void* data)
{
    assert(data != NULL);
    
    _rendererHelper.bindShaderProgram(_shaderProgramWrapper);
    
    _rendererHelper.useScreenBlending();
    
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data), _uniforms[0]);
    
    _rendererHelper.mapTextureVertices(_inputLayout);
}

void NGFramebufferToScreenProgram::unbind()
{
    _rendererHelper.unmapTextureVertices();
    
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    
    _rendererHelper.bindShaderProgram(NULL);
}
