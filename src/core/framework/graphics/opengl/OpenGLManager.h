//
//  OpenGLManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/19/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLManager__
#define __noctisgames__OpenGLManager__

#include "pch.h"

#include <vector>

#define VERTICES_PER_LINE 2
#define VERTICES_PER_RECTANGLE 4
#define INDICES_PER_RECTANGLE 6

#define OGLManager (OpenGLManager::getInstance())

struct GpuTextureWrapper;

typedef float vec4[4];
typedef vec4 mat4x4[4];

static inline void mat4x4_identity(mat4x4 M)
{
    int i, j;
    for (i=0; i<4; ++i)
        for (j=0; j<4; ++j)
            M[i][j] = i==j ? 1.f : 0.f;
}

static inline void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f)
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
    
    std::vector<GLshort> m_indices;
    
    std::vector<GLuint> m_fbos;
    std::vector<GLuint> m_fbo_textures;
    
    std::vector<GpuTextureWrapper *> m_framebuffers;
    
    std::vector<GLfloat> m_textureVertices;
    std::vector<GLfloat> m_colorVertices;
    
    GLuint sb_vbo_object; // For Sprite Batcher
    GLuint gb_vbo_object; // For Geometry Batcher
    
    GLint m_iScreenFBO;
    GLint m_iMaxTextureSize;
    
    mat4x4 m_viewProjectionMatrix;
    
    int m_iScreenWidth;
    int m_iScreenHeight;
    int m_iRenderWidth;
    int m_iRenderHeight;
    int m_iNumFramebuffers;
    
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
