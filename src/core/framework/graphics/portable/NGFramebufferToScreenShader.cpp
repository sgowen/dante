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

NGFramebufferToScreenShader::NGFramebufferToScreenShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, vertexShaderName, fragmentShaderName)
{
    _uniforms.push_back(new NGShaderUniformInput("u_TextureUnit", 0));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
}

void NGFramebufferToScreenShader::bind(void* vertices, void* data1, void* data2, void* data3)
{
    assert(vertices != NULL);
    assert(data1 != NULL);
    
    _rendererHelper.bindNGShader(_shaderProgramWrapper);
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data1), _uniforms[0]);
    
    std::vector<VERTEX_2D>* screenVertices = static_cast<std::vector<VERTEX_2D>* >(vertices);
    _rendererHelper.mapScreenVertices(_inputLayout, *screenVertices);
}

void NGFramebufferToScreenShader::unbind()
{
    _rendererHelper.unmapScreenVertices();
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    _rendererHelper.bindNGShader(NULL);
}
