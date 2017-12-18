//
//  NGGeometryProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/portable/NGGeometryProgram.h"

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <framework/util/macros.h>

NGGeometryProgram::NGGeometryProgram(RendererHelper& inRendererHelper, ShaderProgramWrapper* inShaderProgramWrapper) : ShaderProgram(inRendererHelper, inShaderProgramWrapper)
{
    u_mvp_matrix_location = glGetUniformLocation(_shaderProgramWrapper->_programObjectId, "u_Matrix");
    a_position_location = glGetAttribLocation(_shaderProgramWrapper->_programObjectId, "a_Position");
    a_color_location = glGetAttribLocation(_shaderProgramWrapper->_programObjectId, "a_Color");
}

NGGeometryProgram::~NGGeometryProgram()
{
    _rendererHelper.destroyShaderProgram(_shaderProgramWrapper);
    
    delete _shaderProgramWrapper;
}

void NGGeometryProgram::bind(void* data)
{
    _rendererHelper.bindShaderProgram(_shaderProgramWrapper);
    
    _rendererHelper.useNormalBlending();
    
    _rendererHelper.bindMatrix(u_mvp_matrix_location);
    
    _rendererHelper.mapColorVertices();
    
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(a_position_location);
    
    glVertexAttribPointer(a_color_location, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(a_color_location);
}

void NGGeometryProgram::unbind()
{
    _rendererHelper.unmapColorVertices();
    
    _rendererHelper.bindShaderProgram(NULL);
}
