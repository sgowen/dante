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

NGGeometryShader::NGGeometryShader(RendererHelper& inRendererHelper, NGShaderLoader& inNGShaderLoader, const char* vertexShaderName, const char* fragmentShaderName) : NGShader(inRendererHelper, inNGShaderLoader, vertexShaderName, fragmentShaderName)
{
    _uniforms.push_back(new NGShaderUniformInput("u_Matrix", 64));
    
    _inputLayout.push_back(new NGShaderVarInput("a_Position", 3, 0));
    _inputLayout.push_back(new NGShaderVarInput("a_Color", 4, 3));
    
    load();
}

void NGGeometryShader::bind(void* data)
{
    _rendererHelper.bindNGShader(_shaderProgramWrapper);
    
    _rendererHelper.useNormalBlending();
    
    _rendererHelper.bindMatrix(_uniforms[0]);
    
    _rendererHelper.mapColorVertices(_inputLayout);
}

void NGGeometryShader::unbind()
{
    _rendererHelper.unmapColorVertices();
    
    _rendererHelper.bindNGShader(NULL);
}
