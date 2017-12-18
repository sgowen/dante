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
#include <framework/graphics/portable/ShaderProgramWrapper.h>

ShaderProgram::ShaderProgram(RendererHelper& inRendererHelper, ShaderProgramWrapper* inShaderProgramWrapper) : _rendererHelper(inRendererHelper), _shaderProgramWrapper(inShaderProgramWrapper)
{
    // Empty
}

ShaderProgram::~ShaderProgram()
{
    // Override
}
