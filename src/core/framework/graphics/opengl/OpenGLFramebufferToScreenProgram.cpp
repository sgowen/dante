//
//  OpenGLFramebufferToScreenProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLFramebufferToScreenProgram.h"

#include <framework/graphics/opengl/OpenGLRendererHelper.h>

#include <assert.h>

OpenGLFramebufferToScreenProgram::OpenGLFramebufferToScreenProgram(OpenGLRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : OpenGLProgram(inRendererHelper, vertexShaderName, fragmentShaderName)
{
    u_texture_unit_location = glGetUniformLocation(_programObjectId, "u_TextureUnit");
    a_position_location = glGetAttribLocation(_programObjectId, "a_Position");
}

void OpenGLFramebufferToScreenProgram::bind(void* data)
{
    assert(data != NULL);
    
    OpenGLProgram::bind(data);

    _rendererHelper->useScreenBlending();
    
    // tell the GPU which texture to use
    _rendererHelper->bindTexture(NGTextureSlot_ZERO, static_cast<TextureWrapper*>(data), u_texture_unit_location);

    mapBuffer(_rendererHelper->getSbVboObject(), _rendererHelper->getTextureVertices());

    glVertexAttribPointer(a_position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(a_position_location);
}

void OpenGLFramebufferToScreenProgram::unbind()
{
    _rendererHelper->bindTexture(NGTextureSlot_ZERO, NULL);
    
    unmapBuffer(_rendererHelper->getSbVboObject());

    OpenGLProgram::unbind();
}
