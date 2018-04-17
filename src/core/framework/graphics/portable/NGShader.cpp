//
//  NGShader.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/NGShader.h>

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <framework/file/portable/FileUtil.h>
#include <framework/util/NGSTDUtil.h>

NGShader::NGShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : _rendererHelper(inRendererHelper), _shaderProgramWrapper(NULL)
{
    _vertexShaderFilePath = FileUtil::filePathForShader(vertexShaderName);
    _fragmentShaderFilePath = FileUtil::filePathForShader(fragmentShaderName);
}

NGShader::~NGShader()
{
    NGSTDUtil::cleanUpVectorOfPointers(_uniforms);
    NGSTDUtil::cleanUpVectorOfPointers(_inputLayout);
}

void NGShader::load(ShaderProgramLoader& shaderProgramLoader)
{
    _shaderProgramWrapper = shaderProgramLoader.loadShader(_vertexShaderFilePath, _fragmentShaderFilePath, _uniforms, _inputLayout);
}

void NGShader::unload(ShaderProgramLoader& shaderProgramLoader)
{
    if (_shaderProgramWrapper != NULL)
    {
        shaderProgramLoader.destroyShader(_shaderProgramWrapper, _uniforms, _inputLayout);
        
        delete _shaderProgramWrapper;
        _shaderProgramWrapper = NULL;
    }
}

bool NGShader::isLoaded()
{
    return _shaderProgramWrapper != NULL;
}
