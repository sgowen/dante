//
//  OpenGLGeometryProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLGeometryProgram.h"

#include "OpenGLManager.h"

OpenGLGeometryProgram::OpenGLGeometryProgram(const char* vertexShaderPath, const char* fragmentShaderPath) : OpenGLProgram(vertexShaderPath, fragmentShaderPath)
{
    u_mvp_matrix_location = glGetUniformLocation(m_programObjectId, "u_MvpMatrix");
    a_position_location = glGetAttribLocation(m_programObjectId, "a_Position");
    a_color_location = glGetAttribLocation(m_programObjectId, "a_Color");
}

void OpenGLGeometryProgram::bind()
{
    OpenGLProgram::bind();
    
    glUniformMatrix4fv(u_mvp_matrix_location, 1, GL_FALSE, (GLfloat*)OGLManager->getViewProjectionMatrix());
    
    mapBuffer(OGLManager->getGbVboObject(), OGLManager->getColorVertices());
    
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(0));
    glVertexAttribPointer(a_color_location, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(3 * sizeof(GL_FLOAT)));
    
    glEnableVertexAttribArray(a_position_location);
    glEnableVertexAttribArray(a_color_location);
}

void OpenGLGeometryProgram::unbind()
{
    unmapBuffer(OGLManager->getGbVboObject());
    
    OpenGLProgram::unbind();
}
