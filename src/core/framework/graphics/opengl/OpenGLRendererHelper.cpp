//
//  OpenGLRendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/opengl/OpenGLRendererHelper.h"

#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"

#include "framework/util/NGSTDUtil.h"

#include <assert.h>

OpenGLRendererHelper::OpenGLRendererHelper() : RendererHelper(), _sbVboObject(0), _gbVboObject(0), _screenFBO(0), _maxTextureSize(64)
{
    // Empty
}

OpenGLRendererHelper::~OpenGLRendererHelper()
{
    releaseDeviceDependentResources();
}

void OpenGLRendererHelper::createDeviceDependentResources()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_screenFBO);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
    
    if (_renderWidth > -1
        && _renderHeight > -1
        && _numFramebuffers > -1)
    {
        releaseFramebuffers();
        createFramebufferObjects();
    }
}

void OpenGLRendererHelper::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    _renderWidth = renderWidth;
    _renderHeight = renderHeight;
    _numFramebuffers = numFramebuffers;
    
    glViewport(0, 0, _renderWidth, _renderHeight);
    
    glScissor(0, 0, _renderWidth, _renderHeight);
    glEnable(GL_SCISSOR_TEST);
    
    releaseFramebuffers();
    createFramebufferObjects();
}

void OpenGLRendererHelper::releaseDeviceDependentResources()
{
    releaseFramebuffers();
    
    glDeleteBuffers(1, &_sbVboObject);
    glDeleteBuffers(1, &_gbVboObject);
    
    _textureVertices.clear();
    _colorVertices.clear();
}

void OpenGLRendererHelper::beginFrame()
{
    glEnable(GL_TEXTURE_2D);
    
    glEnable(GL_BLEND);
}

void OpenGLRendererHelper::endFrame()
{
    glDisable(GL_BLEND);
    
    glDisable(GL_TEXTURE_2D);
}

TextureWrapper* OpenGLRendererHelper::getFramebuffer(int index)
{
    _framebuffer->gpuTextureWrapper = _framebuffers[index];
    
    return _framebuffer;
}

inline void mat4x4_identity(mat4x4 M)
{
    int i, j;
    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            M[i][j] = i==j ? 1.f : 0.f;
        }
    }
}

inline void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f)
{
    M[0][0] = 2.f/(r-l);
    M[0][1] = M[0][2] = M[0][3] = 0.f;
    
    M[1][1] = 2.f/(t-b);
    M[1][0] = M[1][2] = M[1][3] = 0.f;
    
    M[2][2] = -2.f/(f-n);
    M[2][0] = M[2][1] = M[2][3] = 0.f;
    
    M[3][0] = -(r+l)/(r-l);
    M[3][1] = -(t+b)/(t-b);
    M[3][2] = -(f+n)/(f-n);
    M[3][3] = 1.f;
}

void OpenGLRendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    mat4x4_identity(_viewProjectionMatrix);
    
    mat4x4_ortho(_viewProjectionMatrix, left, right, bottom, top, -1, 1);
}

void OpenGLRendererHelper::bindToOffscreenFramebuffer(int index)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbos[index]);
    
    glViewport(0, 0, _renderWidth, _renderHeight);
    glScissor(0, 0, _renderWidth, _renderHeight);
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
}

void OpenGLRendererHelper::destroyTexture(GpuTextureWrapper& textureWrapper)
{
    glDeleteTextures(1, &textureWrapper.texture);
}

void OpenGLRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    glDrawArrays(renderPrimitiveType, first, count);
}

void OpenGLRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t count)
{
    glDrawElements(renderPrimitiveType, count, GL_UNSIGNED_SHORT, &_indices[0]);
}

void OpenGLRendererHelper::bindTexture(NGTextureSlot textureSlot, TextureWrapper* textureWrapper, int32_t flags)
{
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, textureWrapper == NULL ? 0 : textureWrapper->gpuTextureWrapper->texture);
    
    if (textureWrapper != NULL && flags != 0)
    {
        glUniform1i(flags, slotIndexForTextureSlot(textureSlot));
    }
}

GLuint& OpenGLRendererHelper::getSbVboObject()
{
    return _sbVboObject;
}

GLuint& OpenGLRendererHelper::getGbVboObject()
{
    return _gbVboObject;
}

mat4x4& OpenGLRendererHelper::getViewProjectionMatrix()
{
    return _viewProjectionMatrix;
}

void OpenGLRendererHelper::createFramebufferObject()
{
    GLuint fbo_texture;
    GLuint fbo;
    
    // Texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
    
    _framebuffers.push_back(new GpuTextureWrapper(fbo_texture));
}

void OpenGLRendererHelper::releaseFramebuffers()
{
    for (std::vector<GLuint>::iterator i = _fbo_textures.begin(); i != _fbo_textures.end(); ++i)
    {
        glDeleteTextures(1, &(*i));
    }
    
    _fbo_textures.clear();
    
    for (std::vector<GLuint>::iterator i = _fbos.begin(); i != _fbos.end(); ++i)
    {
        glDeleteFramebuffers(1, &(*i));
    }
    
    _fbos.clear();
    
    NGSTDUtil::cleanUpVectorOfPointers(_framebuffers);
}

int OpenGLRendererHelper::slotIndexForTextureSlot(NGTextureSlot textureSlot)
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
