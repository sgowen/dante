//
//  NGGeometryShader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGGeometryShader.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/NGShaderLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <assert.h>

NGGeometryShader::NGGeometryShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, vertexShaderName, fragmentShaderName)
{
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix", 64));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 2, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_Color", 4, 2));
}

void NGGeometryShader::bind(void* vertices, void* data1, void* data2)
{
    assert(vertices != NULL);
    
    _rendererHelper.bindNGShader(_shaderProgramWrapper);
    _rendererHelper.bindMatrix(_uniforms[0]);
    
    std::vector<COLOR_VERTEX>* colorVertices = static_cast<std::vector<COLOR_VERTEX>* >(vertices);
    _rendererHelper.mapColorVertices(_inputLayout, *colorVertices);
}

void NGGeometryShader::unbind()
{
    _rendererHelper.unmapColorVertices();
    _rendererHelper.bindNGShader(NULL);
}
