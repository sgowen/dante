//
//  OpenGLRendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/opengl/OpenGLRendererHelper.h>

#include <framework/graphics/portable/NGTexture.h>
#include <framework/graphics/portable/TextureWrapper.h>
#include <framework/graphics/portable/ShaderProgramWrapper.h>
#include <framework/graphics/portable/NGShaderUniformInput.h>
#include <framework/util/Constants.h>

#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/util/Config.h>
#include <framework/util/NGSTDUtil.h>

#include <assert.h>

OpenGLRendererHelper::OpenGLRendererHelper() : RendererHelper(),
_screenFBO(0),
_currentShaderProgramWrapper(NULL)
{
    // Empty
}

OpenGLRendererHelper::~OpenGLRendererHelper()
{
    // Empty
}

void OpenGLRendererHelper::createDeviceDependentResources()
{
    RendererHelper::createDeviceDependentResources();
    
    assert(_offscreenFBOTextures.size() == 0);
    assert(_offscreenFBOs.size() == 0);
    assert(_FBOTextures.size() == 0);
    assert(_FBOs.size() == 0);
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_screenFBO);
}

void OpenGLRendererHelper::bindToOffscreenFramebuffer(int index)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _offscreenFBOs[index]);

    bindToFramebuffer(_offscreenFramebufferDefs[index]);
}

void OpenGLRendererHelper::bindToFramebuffer(int index)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _FBOs[index]);
    
    bindToFramebuffer(_framebufferDefs[index]);
}

void OpenGLRendererHelper::bindToScreenFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _screenFBO);
    
    glViewport(0, 0, _screenWidth, _screenHeight);
    glScissor(0, 0, _screenWidth, _screenHeight);
    glEnable(GL_SCISSOR_TEST);
}

void OpenGLRendererHelper::clearFramebufferWithColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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

void OpenGLRendererHelper::bindShader(ShaderProgramWrapper* shaderProgramWrapper)
{
    glUseProgram(shaderProgramWrapper == NULL ? 0 : shaderProgramWrapper->_programObjectId);
    
    _currentShaderProgramWrapper = shaderProgramWrapper;
}

void OpenGLRendererHelper::bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform)
{
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, texture ? texture->textureWrapper->texture : 0);
    
    if (texture)
    {
        glEnable(GL_TEXTURE_2D);
        glUniform1i(uniform->_attribute, slotIndexForTextureSlot(textureSlot));
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }
}

void OpenGLRendererHelper::mapTextureVertices(std::vector<VERTEX_2D_TEXTURE>& vertices, bool useStaticBuffer, int gpuBufferIndex)
{
    bindTextureVertexBuffer(useStaticBuffer, gpuBufferIndex);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_2D_TEXTURE) * vertices.size(), &vertices[0]);
}

void OpenGLRendererHelper::mapVertices(std::vector<VERTEX_2D>& vertices, bool useStaticBuffer, int gpuBufferIndex)
{
    bindVertexBuffer(useStaticBuffer, gpuBufferIndex);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_2D) * vertices.size(), &vertices[0]);
}

void OpenGLRendererHelper::bindTextureVertexBuffer(bool useStaticBuffer, int gpuBufferIndex)
{
    GLuint& buffer = useStaticBuffer ? _staticTextureVertexBuffers[gpuBufferIndex]->buffer : _dynamicTextureVertexBuffers[gpuBufferIndex]->buffer;
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    bindInputLayout(_currentShaderProgramWrapper->_inputLayout);
}

void OpenGLRendererHelper::bindVertexBuffer(bool useStaticBuffer, int gpuBufferIndex)
{
    GLuint& buffer = useStaticBuffer ? _staticVertexBuffers[gpuBufferIndex]->buffer : _dynamicVertexBuffers[gpuBufferIndex]->buffer;
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    bindInputLayout(_currentShaderProgramWrapper->_inputLayout);
}

void OpenGLRendererHelper::bindScreenVertexBuffer()
{
    GLuint& buffer = _staticScreenVertexBuffer->buffer;
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    bindInputLayout(_currentShaderProgramWrapper->_inputLayout);
}

void OpenGLRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    glDrawArrays(renderPrimitiveType, first, count);
}

void OpenGLRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer->buffer);
    glDrawElements(renderPrimitiveType, count, GL_UNSIGNED_SHORT, (void*)0);
}

GPUBufferWrapper* OpenGLRendererHelper::createGPUBuffer(size_t size, const void *data, bool useStaticBuffer, bool isVertex)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(isVertex ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(isVertex ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, size, data, useStaticBuffer ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    
    return new GPUBufferWrapper(buffer);
}

void OpenGLRendererHelper::disposeGPUBuffer(GPUBufferWrapper* gpuBuffer)
{
    assert(gpuBuffer != NULL);
    
    glDeleteBuffers(1, &gpuBuffer->buffer);
}

TextureWrapper* OpenGLRendererHelper::createOffscreenFramebuffer(int renderWidth, int renderHeight)
{
    GLuint fbo_texture = 0;
    GLuint fbo = 0;
    
    createFramebuffer(fbo_texture, fbo, renderWidth, renderHeight);
    
    _offscreenFBOTextures.push_back(fbo_texture);
    _offscreenFBOs.push_back(fbo);
    
    return new TextureWrapper(fbo_texture);
}

TextureWrapper* OpenGLRendererHelper::createFramebuffer(int renderWidth, int renderHeight)
{
    GLuint fbo_texture = 0;
    GLuint fbo = 0;

    createFramebuffer(fbo_texture, fbo, renderWidth, renderHeight);

    _FBOTextures.push_back(fbo_texture);
    _FBOs.push_back(fbo);

    return new TextureWrapper(fbo_texture);
}

void OpenGLRendererHelper::platformReleaseOffscreenFramebuffers()
{
    for (std::vector<GLuint>::iterator i = _offscreenFBOTextures.begin(); i != _offscreenFBOTextures.end(); ++i)
    {
        glDeleteTextures(1, &(*i));
    }
    _offscreenFBOTextures.clear();
    
    for (std::vector<GLuint>::iterator i = _offscreenFBOs.begin(); i != _offscreenFBOs.end(); ++i)
    {
        glDeleteFramebuffers(1, &(*i));
    }
    _offscreenFBOs.clear();
}

void OpenGLRendererHelper::platformReleaseFramebuffers()
{
    for (std::vector<GLuint>::iterator i = _FBOTextures.begin(); i != _FBOTextures.end(); ++i)
    {
        glDeleteTextures(1, &(*i));
    }
    _FBOTextures.clear();

    for (std::vector<GLuint>::iterator i = _FBOs.begin(); i != _FBOs.end(); ++i)
    {
        glDeleteFramebuffers(1, &(*i));
    }
    _FBOs.clear();
}

void OpenGLRendererHelper::createFramebuffer(GLuint& fbo_texture, GLuint& fbo, int renderWidth, int renderHeight)
{
    std::string cfgFilterMin = FW_CFG->getString("FramebufferFilterMin");
    std::string cfgFilterMag = FW_CFG->getString("FramebufferFilterMag");
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    glBindFramebuffer(GL_FRAMEBUFFER, _screenFBO);
    
    assert(status == GL_FRAMEBUFFER_COMPLETE);
}

void OpenGLRendererHelper::bindToFramebuffer(FramebufferDef& framebufferDef)
{
    FramebufferDef& fbd = framebufferDef;
    
    glViewport(0, 0, fbd.renderWidth, fbd.renderHeight);
    glScissor(0, 0, fbd.renderWidth, fbd.renderHeight);
    glEnable(GL_SCISSOR_TEST);
}

void OpenGLRendererHelper::bindInputLayout(std::vector<NGShaderVarInput*>& inputLayout)
{
    for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
    {
        NGShaderVarInput* svi = (*i);
        
        glVertexAttribPointer(svi->_attribute, svi->_size, GL_FLOAT, GL_FALSE, svi->_stride, svi->_bufferOffset);
    }
}
