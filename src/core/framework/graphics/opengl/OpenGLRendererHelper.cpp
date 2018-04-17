//
//  OpenGLRendererHelper.cpp
//  noctisgames
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
#include <framework/graphics/portable/FramebufferWrapper.h>
#include <framework/graphics/portable/NGShaderVarInput.h>

#include <framework/graphics/portable/NGTextureDesc.h>
#include <framework/graphics/portable/NGShaderVarInput.h>
#include <framework/util/Config.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/util/Constants.h>

#include <assert.h>

OpenGLRendererHelper::OpenGLRendererHelper() : RendererHelper(),
_currentShaderProgramWrapper(NULL)
{
    // Empty
}

OpenGLRendererHelper::~OpenGLRendererHelper()
{
    // Empty
}

void OpenGLRendererHelper::clearFramebufferWithColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRendererHelper::useNormalBlending()
{
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

void OpenGLRendererHelper::useScreenBlending()
{
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

void OpenGLRendererHelper::disableBlending()
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
    glBindTexture(GL_TEXTURE_2D, texture ? texture->_textureWrapper->texture : 0);
    
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

void OpenGLRendererHelper::createScreenFramebufferWrapper(FramebufferWrapper* fbw)
{
    GLint screenFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &screenFBO);
    
    fbw->fbo = screenFBO;
}

TextureWrapper* OpenGLRendererHelper::createFramebufferImpl(FramebufferWrapper* fbw)
{
    GLuint& fbo_texture = fbw->fboTexture;
    GLuint& fbo = fbw->fbo;
    
    NGTextureDesc* td = fbw->texture->_desc;
    std::string cfgFilterMin = td->_textureFilterMin;
    std::string cfgFilterMag = td->_textureFilterMag;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbw->renderWidth, fbw->renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    assert(status == GL_FRAMEBUFFER_COMPLETE);
    
    return new TextureWrapper(fbo_texture);
}

void OpenGLRendererHelper::destroyFramebufferImpl(FramebufferWrapper* fbw)
{
    GLuint& fbo_texture = fbw->fboTexture;
    GLuint& fbo = fbw->fbo;
    
    glDeleteTextures(1, &fbo_texture);
    glDeleteFramebuffers(1, &fbo);
}

void OpenGLRendererHelper::bindFramebufferImpl(FramebufferWrapper* fbw)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbw->fbo);
}

void OpenGLRendererHelper::bindViewport(int renderWidth, int renderHeight)
{
    glViewport(0, 0, renderWidth, renderHeight);
    glScissor(0, 0, renderWidth, renderHeight);
    glEnable(GL_SCISSOR_TEST);
}

GPUBufferWrapper* OpenGLRendererHelper::createGPUBufferImpl(size_t size, const void *data, bool useStaticBuffer, bool isVertex)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(isVertex ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(isVertex ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, size, data, useStaticBuffer ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    
    return new GPUBufferWrapper(buffer);
}

void OpenGLRendererHelper::destroyGPUBufferImpl(GPUBufferWrapper* gpuBuffer)
{
    assert(gpuBuffer != NULL);
    
    glDeleteBuffers(1, &gpuBuffer->buffer);
}

void OpenGLRendererHelper::bindInputLayout(std::vector<NGShaderVarInput*>& inputLayout)
{
    for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
    {
        NGShaderVarInput* svi = (*i);
        
        glVertexAttribPointer(svi->_attribute, svi->_size, GL_FLOAT, GL_FALSE, svi->_stride, svi->_bufferOffset);
    }
}
