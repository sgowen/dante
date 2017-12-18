//
//  OpenGLTextureProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLTextureProgram.h"

#include <framework/graphics/opengl/OpenGLRendererHelper.h>

#include <assert.h>

OpenGLTextureProgram::OpenGLTextureProgram(OpenGLRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : OpenGLProgram(inRendererHelper, vertexShaderName, fragmentShaderName)
{
    u_mvp_matrix_location = glGetUniformLocation(_programObjectId, "u_MvpMatrix");
    u_texture_unit_location = glGetUniformLocation(_programObjectId, "u_TextureUnit");
    a_position_location = glGetAttribLocation(_programObjectId, "a_Position");
    a_color_location = glGetAttribLocation(_programObjectId, "a_Color");
    a_texture_coordinates_location = glGetAttribLocation(_programObjectId, "a_TextureCoordinates");
}

void OpenGLTextureProgram::bind(void* data)
{
    assert(data != NULL);
    
    glUseProgram(_programObjectId);
    
    _rendererHelper->useNormalBlending();
    
    glUniformMatrix4fv(u_mvp_matrix_location, 1, GL_FALSE, (GLfloat*)_rendererHelper->getViewProjectionMatrix());
    
    // tell the GPU which texture to use
    _rendererHelper->bindTexture(NGTextureSlot_ZERO, static_cast<TextureWrapper*>(data), u_texture_unit_location);
    
    GLuint vbo = _rendererHelper->getSbVboObject();
    std::vector<TEXTURE_VERTEX>& vertices = _rendererHelper->getTextureVertices();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TEXTURE_VERTEX) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(0));
    glVertexAttribPointer(a_color_location, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(3 * sizeof(GL_FLOAT)));
    glVertexAttribPointer(a_texture_coordinates_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(7 * sizeof(GL_FLOAT)));
    
    glEnableVertexAttribArray(a_position_location);
    glEnableVertexAttribArray(a_color_location);
    glEnableVertexAttribArray(a_texture_coordinates_location);
}

void OpenGLTextureProgram::unbind()
{
    _rendererHelper->bindTexture(NGTextureSlot_ZERO, NULL);
    
    GLuint vbo = _rendererHelper->getSbVboObject();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    
    glUseProgram(0);
}
