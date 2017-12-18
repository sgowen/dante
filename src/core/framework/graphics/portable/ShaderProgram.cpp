//
//  ShaderProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/ShaderProgram.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <framework/util/NGSTDUtil.h>

ShaderProgram::ShaderProgram(RendererHelper& inRendererHelper, ShaderProgramLoader& inShaderProgramLoader, const char* vertexShaderName, const char* fragmentShaderName) : _rendererHelper(inRendererHelper), _shaderProgramLoader(inShaderProgramLoader), _vertexShaderName(vertexShaderName), _fragmentShaderName(fragmentShaderName), _shaderProgramWrapper(NULL)
{
    // Empty
}

ShaderProgram::~ShaderProgram()
{
    if (_shaderProgramWrapper != NULL)
    {
        _rendererHelper.destroyShaderProgram(_shaderProgramWrapper);
        
        delete _shaderProgramWrapper;
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(_uniforms);
    NGSTDUtil::cleanUpVectorOfPointers(_inputLayout);
}

void ShaderProgram::load()
{
    _shaderProgramWrapper = _shaderProgramLoader.loadShaderProgram(_vertexShaderName, _fragmentShaderName, _uniforms, _inputLayout);
}
