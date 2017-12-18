//
//  NGTextureProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/portable/NGTextureProgram.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <framework/util/macros.h>

#include <assert.h>

NGTextureProgram::NGTextureProgram(RendererHelper& inRendererHelper, ShaderProgramWrapper* inShaderProgramWrapper) : ShaderProgram(inRendererHelper, inShaderProgramWrapper)
{
    u_mvp_matrix_location = glGetUniformLocation(_shaderProgramWrapper->_programObjectId, "u_Matrix");
    u_texture_unit_location = glGetUniformLocation(_shaderProgramWrapper->_programObjectId, "u_TextureUnit");
    a_position_location = glGetAttribLocation(_shaderProgramWrapper->_programObjectId, "a_Position");
    a_color_location = glGetAttribLocation(_shaderProgramWrapper->_programObjectId, "a_Color");
    a_texture_coordinates_location = glGetAttribLocation(_shaderProgramWrapper->_programObjectId, "a_TexCoord");
}

NGTextureProgram::~NGTextureProgram()
{
    _rendererHelper.destroyShaderProgram(_shaderProgramWrapper);
    
    delete _shaderProgramWrapper;
}

void NGTextureProgram::bind(void* data)
{
    assert(data != NULL);
    
    _rendererHelper.bindShaderProgram(_shaderProgramWrapper);
    
    _rendererHelper.useNormalBlending();
    
    _rendererHelper.bindMatrix(u_mvp_matrix_location);
    
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data), u_texture_unit_location);
    
    _rendererHelper.mapTextureVertices();
    
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(a_position_location);
    
    glVertexAttribPointer(a_color_location, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(a_color_location);
    
    glVertexAttribPointer(a_texture_coordinates_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(7 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(a_texture_coordinates_location);
}

void NGTextureProgram::unbind()
{
    _rendererHelper.bindTexture(NGTextureSlot_ZERO, NULL);
    
    _rendererHelper.unmapTextureVertices();
    
    _rendererHelper.bindShaderProgram(NULL);
}
