//
//  OpenGLTextureProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLTextureProgram.h"

#include "OpenGLManager.h"

OpenGLTextureProgram::OpenGLTextureProgram(const char* vertexShaderName, const char* fragmentShaderName) : OpenGLProgram(vertexShaderName, fragmentShaderName)
{
    u_mvp_matrix_location = glGetUniformLocation(m_programObjectId, "u_MvpMatrix");
    u_texture_unit_location = glGetUniformLocation(m_programObjectId, "u_TextureUnit");
    a_position_location = glGetAttribLocation(m_programObjectId, "a_Position");
    a_color_location = glGetAttribLocation(m_programObjectId, "a_Color");
    a_texture_coordinates_location = glGetAttribLocation(m_programObjectId, "a_TextureCoordinates");
}

void OpenGLTextureProgram::bind()
{
    OpenGLProgram::bind();
    
    glUniformMatrix4fv(u_mvp_matrix_location, 1, GL_FALSE, (GLfloat*)OGLManager->getViewProjectionMatrix());
    
    glUniform1i(u_texture_unit_location, 0);
    
    mapBuffer(OGLManager->getSbVboObject(), OGLManager->getTextureVertices());
    
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(0));
    glVertexAttribPointer(a_color_location, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(3 * sizeof(GL_FLOAT)));
    glVertexAttribPointer(a_texture_coordinates_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(7 * sizeof(GL_FLOAT)));
    
    glEnableVertexAttribArray(a_position_location);
    glEnableVertexAttribArray(a_color_location);
    glEnableVertexAttribArray(a_texture_coordinates_location);
}

void OpenGLTextureProgram::unbind()
{
    unmapBuffer(OGLManager->getSbVboObject());
    
    OpenGLProgram::unbind();
}
