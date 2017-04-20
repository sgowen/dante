//
//  OpenGLFramebufferToScreenProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLFramebufferToScreenProgram.h"

#include "OpenGLManager.h"

OpenGLFramebufferToScreenProgram::OpenGLFramebufferToScreenProgram(const char* vertexShaderPath, const char* fragmentShaderPath) : OpenGLProgram(vertexShaderPath, fragmentShaderPath)
{
    u_texture_unit_location = glGetUniformLocation(m_programObjectId, "u_TextureUnit");
    a_position_location = glGetAttribLocation(m_programObjectId, "a_Position");
}

void OpenGLFramebufferToScreenProgram::bind()
{
    OpenGLProgram::bind();
    
    glUniform1i(u_texture_unit_location, 0);
    
    mapBuffer(OGLManager->getSbVboObject(), OGLManager->getTextureVertices());
    
    glVertexAttribPointer(a_position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(0));
    
    glEnableVertexAttribArray(a_position_location);
}

void OpenGLFramebufferToScreenProgram::unbind()
{
    unmapBuffer(OGLManager->getSbVboObject());
    
    OpenGLProgram::unbind();
}
