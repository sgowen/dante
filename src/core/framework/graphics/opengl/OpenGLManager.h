//
//  OpenGLManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/19/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLManager__
#define __noctisgames__OpenGLManager__

#include "framework/graphics/portable/NGGraphics.h"

#include <vector>

#define VERTICES_PER_LINE 2
#define VERTICES_PER_RECTANGLE 4
#define INDICES_PER_RECTANGLE 6

#define OGLManager (OpenGLManager::getInstance())

struct GpuTextureWrapper;

typedef float vec4[4];
typedef vec4 mat4x4[4];

class OpenGLManager
{
public:
    static void create();
    
    static void destroy();
    
    static OpenGLManager * getInstance();
    
    void createDeviceDependentResources(int maxBatchSize);
    
    void createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers);
    
    void releaseDeviceDependentResources();
    
    void createMatrix(float left, float right, float bottom, float top);
    
    // Called by Batchers
    void addVertexCoordinate(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat u, GLfloat v);
    void addVertexCoordinate(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    
    void useNormalBlending();
    void useScreenBlending();
    
    void setScreenSize(int screenWidth, int screenHeight);
    
    std::vector<GLshort>& getIndices();
    std::vector<GLuint>& getFbos();
    std::vector<GLuint>& getFboTextures();
    std::vector<GpuTextureWrapper *>& getFramebuffers();
    std::vector<GLfloat>& getTextureVertices();
    std::vector<GLfloat>& getColorVertices();
    GLuint& getSbVboObject(); // For Sprite Batcher
    GLuint& getGbVboObject(); // For Geometry Batcher
    GLint& getScreenFBO();
    GLint& getMaxTextureSize();
    mat4x4& getViewProjectionMatrix();
    int getScreenWidth();
    int getScreenHeight();
    int getRenderWidth();
    int getRenderHeight();
    
private:
    static OpenGLManager* s_pInstance;
    
    std::vector<GLshort> _indices;
    
    std::vector<GLuint> _fbos;
    std::vector<GLuint> _fbo_textures;
    
    std::vector<GpuTextureWrapper *> _framebuffers;
    
    std::vector<GLfloat> _textureVertices;
    std::vector<GLfloat> _colorVertices;
    
    GLuint sb_vbo_object; // For Sprite Batcher
    GLuint gb_vbo_object; // For Geometry Batcher
    
    GLint _screenFBO;
    GLint _maxTextureSize;
    
    mat4x4 _viewProjectionMatrix;
    
    int _screenWidth;
    int _screenHeight;
    int _renderWidth;
    int _renderHeight;
    int _numFramebuffers;
    
    void generateIndices(int maxBatchSize);
    void createFramebufferObjects();
    void createFramebufferObject();
    void releaseFramebuffers();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    OpenGLManager();
    ~OpenGLManager();
    OpenGLManager(const OpenGLManager&);
    OpenGLManager& operator=(const OpenGLManager&);
};

#endif /* defined(__noctisgames__OpenGLManager__) */
