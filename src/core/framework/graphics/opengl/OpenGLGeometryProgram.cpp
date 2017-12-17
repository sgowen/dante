//
//  OpenGLGeometryProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLGeometryProgram.h"

#include <framework/graphics/opengl/OpenGLRendererHelper.h>

OpenGLGeometryProgram::OpenGLGeometryProgram(OpenGLRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : OpenGLProgram(inRendererHelper, vertexShaderName, fragmentShaderName)
{
    u_mvp_matrix_location = glGetUniformLocation(_programObjectId, "u_MvpMatrix");
    a_position_location = glGetAttribLocation(_programObjectId, "a_Position");
    a_color_location = glGetAttribLocation(_programObjectId, "a_Color");
}

void OpenGLGeometryProgram::bind()
{
    OpenGLProgram::bind();
    
    _rendererHelper->useNormalBlending();
    
    glUniformMatrix4fv(u_mvp_matrix_location, 1, GL_FALSE, (GLfloat*)_rendererHelper->getViewProjectionMatrix());
    
    mapBuffer(_rendererHelper->getGbVboObject(), _rendererHelper->getColorVertices());
    
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(0));
    glVertexAttribPointer(a_color_location, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(3 * sizeof(GL_FLOAT)));
    
    glEnableVertexAttribArray(a_position_location);
    glEnableVertexAttribArray(a_color_location);
}

void OpenGLGeometryProgram::unbind()
{
    unmapBuffer(_rendererHelper->getGbVboObject());
    
    OpenGLProgram::unbind();
}
