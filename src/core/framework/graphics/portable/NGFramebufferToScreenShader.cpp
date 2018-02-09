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
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <assert.h>

NGFramebufferToScreenShader::NGFramebufferToScreenShader(RendererHelper& inRendererHelper) : NGShader(inRendererHelper, "shader_002_vert.ngs", "shader_002_frag.ngs")
{
    // Textures
    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit", 0));
    
    // Vertices
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
}

void NGFramebufferToScreenShader::bind(void* data1, void* data2, void* data3)
{
    assert(data1 != NULL);
    
    _rendererHelper.bindShader(_shaderProgramWrapper);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[0]);
}

void NGFramebufferToScreenShader::unbind()
{
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    _rendererHelper.bindShader(NULL);
}
