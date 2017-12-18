//
//  NGShaderUniformInput.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGShaderUniformInput.h"

#include "framework/graphics/portable/ShaderProgramWrapper.h"

NGShaderUniformInput::NGShaderUniformInput(const char* attribName) : _attribName(attribName)
{
    // Empty
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
void NGShaderUniformInput::build(ShaderProgramWrapper* inShaderProgramWrapper)
{
    _attribute = glGetUniformLocation(inShaderProgramWrapper->_programObjectId, _attribName);
}
#endif
