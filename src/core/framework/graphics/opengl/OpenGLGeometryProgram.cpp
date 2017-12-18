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
    u_mvp_matrix_location = glGetUniformLocation(_programObjectId, "u_Matrix");
    a_position_location = glGetAttribLocation(_programObjectId, "a_Position");
    a_color_location = glGetAttribLocation(_programObjectId, "a_Color");
}

void OpenGLGeometryProgram::bind(void* data)
{
    glUseProgram(_programObjectId);
    
    _rendererHelper->useNormalBlending();
    
    glUniformMatrix4fv(u_mvp_matrix_location, 1, GL_FALSE, (GLfloat*)_rendererHelper->getViewProjectionMatrix());
    
    GLuint vbo = _rendererHelper->getGbVboObject();
    std::vector<COLOR_VERTEX>& vertices = _rendererHelper->getColorVertices();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(COLOR_VERTEX) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(0));
    glVertexAttribPointer(a_color_location, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(3 * sizeof(GL_FLOAT)));
    
    glEnableVertexAttribArray(a_position_location);
    glEnableVertexAttribArray(a_color_location);
}

void OpenGLGeometryProgram::unbind()
{
    GLuint vbo = _rendererHelper->getGbVboObject();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    
    glUseProgram(0);
}
