//
//  OpenGLManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/19/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLManager.h"

#include "framework/graphics/opengl/OpenGLTextureGpuProgramWrapper.h"
#include "framework/graphics/opengl/OpenGLGeometryGpuProgramWrapper.h"
#include "framework/graphics/opengl/OpenGLFramebufferToScreenGpuProgramWrapper.h"
#include "framework/graphics/opengl/OpenGLGeometryProgram.h"
#include "framework/graphics/opengl/OpenGLTextureProgram.h"
#include "framework/graphics/opengl/OpenGLFramebufferToScreenProgram.h"
#include "framework/util/macros.h"
#include "framework/util/NGSTDUtil.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"

#include <assert.h>

OpenGLManager* OpenGLManager::s_pInstance = nullptr;

void OpenGLManager::create()
{
    assert(!s_pInstance);
    
    s_pInstance = new OpenGLManager();
}

void OpenGLManager::destroy()
{
    assert(s_pInstance);
    
    delete s_pInstance;
    s_pInstance = nullptr;
}

OpenGLManager * OpenGLManager::getInstance()
{
    return s_pInstance;
}

void OpenGLManager::createDeviceDependentResources(int maxBatchSize)
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_screenFBO);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
    
    generateIndices(maxBatchSize);
    
    if (_renderWidth > -1
        && _renderHeight > -1
        && _numFramebuffers > -1)
    {
        releaseFramebuffers();
        createFramebufferObjects();
    }
}

void OpenGLManager::createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers)
{
    _renderWidth = renderWidth;
    _renderHeight = renderHeight;
    _numFramebuffers = numFramebuffers;
    
    glViewport(0, 0, _renderWidth, _renderHeight);
    
    glScissor(0, 0, _renderWidth, _renderHeight);
    glEnable(GL_SCISSOR_TEST);
    
    releaseFramebuffers();
    createFramebufferObjects();
}

void OpenGLManager::releaseDeviceDependentResources()
{
    _indices.clear();
    
    releaseFramebuffers();
    
    glDeleteBuffers(1, &sb_vbo_object);
    glDeleteBuffers(1, &gb_vbo_object);
}

void OpenGLManager::createMatrix(float left, float right, float bottom, float top)
{
    mat4x4_identity(_viewProjectionMatrix);
    
    mat4x4_ortho(_viewProjectionMatrix, left, right, bottom, top, -1, 1);
}

void OpenGLManager::addVertexCoordinate(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat u, GLfloat v)
{
    _textureVertices.push_back(x);
    _textureVertices.push_back(y);
    _textureVertices.push_back(z);
    _textureVertices.push_back(r);
    _textureVertices.push_back(g);
    _textureVertices.push_back(b);
    _textureVertices.push_back(a);
    _textureVertices.push_back(u);
    _textureVertices.push_back(v);
}

void OpenGLManager::addVertexCoordinate(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    _colorVertices.push_back(x);
    _colorVertices.push_back(y);
    _colorVertices.push_back(z);
    _colorVertices.push_back(r);
    _colorVertices.push_back(g);
    _colorVertices.push_back(b);
    _colorVertices.push_back(a);
}

void OpenGLManager::useNormalBlending()
{
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

void OpenGLManager::useScreenBlending()
{
    glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

void OpenGLManager::setScreenSize(int screenWidth, int screenHeight)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
}

std::vector<GLshort>& OpenGLManager::getIndices()
{
    return _indices;
}

std::vector<GLuint>& OpenGLManager::getFbos()
{
    return _fbos;
}

std::vector<GLuint>& OpenGLManager::getFboTextures()
{
    return _fbo_textures;
}

std::vector<GpuTextureWrapper *>& OpenGLManager::getFramebuffers()
{
    return _framebuffers;
}

std::vector<GLfloat>& OpenGLManager::getTextureVertices()
{
    return _textureVertices;
}

std::vector<GLfloat>& OpenGLManager::getColorVertices()
{
    return _colorVertices;
}

GLuint& OpenGLManager::getSbVboObject()
{
    return sb_vbo_object;
}

GLuint& OpenGLManager::getGbVboObject()
{
    return gb_vbo_object;
}

GLint& OpenGLManager::getScreenFBO()
{
    return _screenFBO;
}

GLint& OpenGLManager::getMaxTextureSize()
{
    return _maxTextureSize;
}

mat4x4& OpenGLManager::getViewProjectionMatrix()
{
    return _viewProjectionMatrix;
}

int OpenGLManager::getScreenWidth()
{
    return _screenWidth;
}

int OpenGLManager::getScreenHeight()
{
    return _screenHeight;
}

int OpenGLManager::getRenderWidth()
{
    return _renderWidth;
}

int OpenGLManager::getRenderHeight()
{
    return _renderHeight;
}

#pragma mark private

void OpenGLManager::generateIndices(int maxBatchSize)
{
    _indices.reserve(maxBatchSize * INDICES_PER_RECTANGLE);
    
    GLshort j = 0;
    for (int i = 0; i < maxBatchSize * INDICES_PER_RECTANGLE; i += INDICES_PER_RECTANGLE, j += VERTICES_PER_RECTANGLE)
    {
        _indices.push_back(j);
        _indices.push_back(j + 1);
        _indices.push_back(j + 2);
        _indices.push_back(j + 2);
        _indices.push_back(j + 3);
        _indices.push_back(j + 0);
    }
}

void OpenGLManager::createFramebufferObjects()
{
    for (int i = 0; i < _numFramebuffers; ++i)
    {
        createFramebufferObject();
    }
}

void OpenGLManager::createFramebufferObject()
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

void OpenGLManager::releaseFramebuffers()
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

OpenGLManager::OpenGLManager() : sb_vbo_object(0), gb_vbo_object(0), _screenFBO(0), _maxTextureSize(64), _renderWidth(-1), _renderHeight(-1), _numFramebuffers(-1)
{
    // Hide Constructor for Singleton
}

OpenGLManager::~OpenGLManager()
{
    releaseDeviceDependentResources();
}
