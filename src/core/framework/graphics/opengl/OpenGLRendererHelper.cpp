//
//  OpenGLRendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/opengl/OpenGLRendererHelper.h"

#include "framework/graphics/portable/NGTexture.h"
#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/ShaderProgramWrapper.h"
#include "framework/graphics/portable/NGShaderUniformInput.h"
#include <framework/util/FrameworkConstants.h>

#include <framework/util/Config.h>
#include "framework/util/NGSTDUtil.h"

#include <assert.h>

OpenGLRendererHelper::OpenGLRendererHelper() : RendererHelper(), _textureVertexBuffer(0), _basicVertexBuffer(0), _screenFBO(0)
{
    // Empty
}

OpenGLRendererHelper::~OpenGLRendererHelper()
{
    releaseDeviceDependentResources();
}

void OpenGLRendererHelper::createDeviceDependentResources()
{
    assert(_fbos.size() == 0);
    assert(_fbo_textures.size() == 0);
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_screenFBO);
    
    createVertexBuffer<VERTEX_2D_TEXTURE>(_textureVertexBuffer, MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    createVertexBuffer<VERTEX_2D>(_basicVertexBuffer, MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    createIndexBuffer();
}

void OpenGLRendererHelper::releaseDeviceDependentResources()
{
    platformReleaseFramebuffers();
    
    glDeleteBuffers(1, &_textureVertexBuffer);
    glDeleteBuffers(1, &_basicVertexBuffer);
    glDeleteBuffers(1, &_indexBuffer);
}

void OpenGLRendererHelper::bindToOffscreenFramebuffer(int index)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbos[index]);

    glViewport(0, 0, _renderWidth, _renderHeight);
    glScissor(0, 0, _renderWidth, _renderHeight);
    glEnable(GL_SCISSOR_TEST);
}

void OpenGLRendererHelper::clearFramebufferWithColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRendererHelper::bindToScreenFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _screenFBO);

    glViewport(0, 0, _screenWidth, _screenHeight);
    glScissor(0, 0, _screenWidth, _screenHeight);
    glEnable(GL_SCISSOR_TEST);
}

void OpenGLRendererHelper::useNormalBlending()
{
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glEnable(GL_BLEND);
}

void OpenGLRendererHelper::useScreenBlending()
{
    glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glEnable(GL_BLEND);
}

void OpenGLRendererHelper::useNoBlending()
{
    glDisable(GL_BLEND);
}

void OpenGLRendererHelper::bindInt4(NGShaderUniformInput* uniform, int4& inValue)
{
    glUniform4i(uniform->_attribute, inValue[0], inValue[1], inValue[2], inValue[3]);
}

void OpenGLRendererHelper::bindFloat4(NGShaderUniformInput* uniform, float4& inValue)
{
    glUniform4f(uniform->_attribute, inValue[0], inValue[1], inValue[2], inValue[3]);
}

void OpenGLRendererHelper::bindFloat4Array(NGShaderUniformInput* uniform, int count, float4* inValue)
{
    glUniform4fv(uniform->_attribute, count, (const GLfloat*)inValue);
}

void OpenGLRendererHelper::bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue)
{
    glUniformMatrix4fv(uniform->_attribute, 1, GL_FALSE, (GLfloat*)inValue);
}

void OpenGLRendererHelper::bindMatrix(NGShaderUniformInput* uniform)
{
    glUniformMatrix4fv(uniform->_attribute, 1, GL_FALSE, (GLfloat*)_matrix);
}

inline int slotIndexForTextureSlot(NGTextureSlot textureSlot)
{
    switch (textureSlot)
    {
        case NGTextureSlot_ZERO:
            return 0;
        case NGTextureSlot_ONE:
            return 1;
    }

    assert(false);
}

void OpenGLRendererHelper::bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform)
{
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, texture ? texture->textureWrapper->texture : 0);
    
    if (texture)
    {
        glEnable(GL_TEXTURE_2D);
        
        if (uniform)
        {
            glUniform1i(uniform->_attribute, slotIndexForTextureSlot(textureSlot));
        }
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }
}

void OpenGLRendererHelper::bindNGShader(ShaderProgramWrapper* shaderProgramWrapper)
{
    glUseProgram(shaderProgramWrapper == NULL ? 0 : shaderProgramWrapper->_programObjectId);
}

void OpenGLRendererHelper::mapTextureVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D_TEXTURE>& vertices)
{
    mapVertices(_textureVertexBuffer, vertices, inputLayout);
}

void OpenGLRendererHelper::mapBasicVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D>& vertices)
{
    mapVertices(_basicVertexBuffer, vertices, inputLayout);
}

void OpenGLRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    glDrawArrays(renderPrimitiveType, first, count);
}

void OpenGLRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glDrawElements(renderPrimitiveType, count, GL_UNSIGNED_SHORT, (void*)0);
}

TextureWrapper* OpenGLRendererHelper::createFramebuffer()
{
    GLuint fbo_texture;
    GLuint fbo;

    std::string cfgFilterMin = NG_CFG->getString("FramebufferFilterMin");
    std::string cfgFilterMag = NG_CFG->getString("FramebufferFilterMag");
    GLint filterMin = cfgFilterMin == "NEAREST" ? GL_NEAREST : GL_LINEAR;
    GLint filterMag = cfgFilterMag == "NEAREST" ? GL_NEAREST : GL_LINEAR;
    
    // Texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _renderWidth, _renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    glBindFramebuffer(GL_FRAMEBUFFER, _screenFBO);

    assert(status == GL_FRAMEBUFFER_COMPLETE);

    _fbo_textures.push_back(fbo_texture);
    _fbos.push_back(fbo);

    return new TextureWrapper(fbo_texture);
}

void OpenGLRendererHelper::platformReleaseFramebuffers()
{
    for (std::vector<GLuint>::iterator i = _fbo_textures.begin(); i != _fbo_textures.end(); ++i)
    {
        glDeleteTextures(1, &(*i));
    }

    for (std::vector<GLuint>::iterator i = _fbos.begin(); i != _fbos.end(); ++i)
    {
        glDeleteFramebuffers(1, &(*i));
    }
    
    _fbo_textures.clear();
    _fbos.clear();
}

void OpenGLRendererHelper::createIndexBuffer()
{
    std::vector<uint16_t> indices;
    indices.reserve(MAX_BATCH_SIZE * INDICES_PER_RECTANGLE);
    
    uint16_t j = 0;
    for (int i = 0; i < MAX_BATCH_SIZE * INDICES_PER_RECTANGLE; i += INDICES_PER_RECTANGLE, j += VERTICES_PER_RECTANGLE)
    {
        indices.push_back(j);
        indices.push_back(j + 1);
        indices.push_back(j + 2);
        indices.push_back(j + 2);
        indices.push_back(j + 3);
        indices.push_back(j + 0);
    }
    
    glGenBuffers(1, &_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), &indices[0], GL_STATIC_DRAW);
}
