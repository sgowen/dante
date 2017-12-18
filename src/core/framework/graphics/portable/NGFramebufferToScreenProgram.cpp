//
//  NGFramebufferToScreenProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/portable/NGFramebufferToScreenProgram.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <framework/util/macros.h>

#include <assert.h>

NGFramebufferToScreenProgram::NGFramebufferToScreenProgram(RendererHelper& inRendererHelper, ShaderProgramWrapper* inShaderProgramWrapper) : ShaderProgram(inRendererHelper, inShaderProgramWrapper)
{
    u_texture_unit_location = glGetUniformLocation(_shaderProgramWrapper->_programObjectId, "u_TextureUnit");
    a_position_location = glGetAttribLocation(_shaderProgramWrapper->_programObjectId, "a_Position");
}

NGFramebufferToScreenProgram::~NGFramebufferToScreenProgram()
{
    _rendererHelper.destroyShaderProgram(_shaderProgramWrapper);
    
    delete _shaderProgramWrapper;
}

void NGFramebufferToScreenProgram::bind(void* data)
{
    assert(data != NULL);
    
    _rendererHelper.bindShaderProgram(_shaderProgramWrapper);
    
    _rendererHelper.useScreenBlending();
    
    // tell the GPU which texture to use
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data), u_texture_unit_location);
    
    _rendererHelper.mapTextureVertices();
    
    glVertexAttribPointer(a_position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(a_position_location);
}

void NGFramebufferToScreenProgram::unbind()
{
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    
    _rendererHelper.unmapTextureVertices();
    
    _rendererHelper.bindShaderProgram(NULL);
}
