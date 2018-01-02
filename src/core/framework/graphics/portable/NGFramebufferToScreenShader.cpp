//
//  NGFramebufferToScreenShader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGFramebufferToScreenShader.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/NGShaderLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <assert.h>

NGFramebufferToScreenShader::NGFramebufferToScreenShader(RendererHelper& inRendererHelper, NGShaderLoader& inNGShaderLoader, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, inNGShaderLoader, vertexShaderName, fragmentShaderName)
{
    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit"));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    
    load();
}

void NGFramebufferToScreenShader::bind(void* data)
{
    assert(data != NULL);
    
    _rendererHelper.bindNGShader(_shaderProgramWrapper);
    
    _rendererHelper.useScreenBlending();
    
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data), _uniforms[0]);
    
    _rendererHelper.mapScreenVertices(_inputLayout);
}

void NGFramebufferToScreenShader::unbind()
{
    _rendererHelper.unmapScreenVertices();
    
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    
    _rendererHelper.bindNGShader(NULL);
}
