//
//  NGShader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGShader.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/NGShaderLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <framework/util/NGSTDUtil.h>

NGShader::NGShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : _rendererHelper(inRendererHelper), _vertexShaderName(vertexShaderName), _fragmentShaderName(fragmentShaderName), _shaderProgramWrapper(NULL)
{
    // Empty
}

NGShader::~NGShader()
{
    NGSTDUtil::cleanUpVectorOfPointers(_uniforms);
    NGSTDUtil::cleanUpVectorOfPointers(_inputLayout);
}

void NGShader::load(NGShaderLoader& shaderProgramLoader)
{
    _shaderProgramWrapper = shaderProgramLoader.loadNGShader(_vertexShaderName, _fragmentShaderName, _uniforms, _inputLayout);
}

void NGShader::unload(NGShaderLoader& shaderProgramLoader)
{
    if (_shaderProgramWrapper != NULL)
    {
        shaderProgramLoader.destroyNGShader(_shaderProgramWrapper, _uniforms, _inputLayout);
        
        delete _shaderProgramWrapper;
        
        _shaderProgramWrapper = NULL;
    }
}

bool NGShader::isLoaded()
{
    return _shaderProgramWrapper != NULL;
}
