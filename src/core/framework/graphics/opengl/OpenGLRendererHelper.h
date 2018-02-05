//
//  OpenGLRendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLRendererHelper__
#define __noctisgames__OpenGLRendererHelper__

#include "framework/graphics/portable/RendererHelper.h"

#include "framework/graphics/portable/NGShaderVarInput.h"

class OpenGLRendererHelper : public RendererHelper
{
public:
    OpenGLRendererHelper();
    virtual ~OpenGLRendererHelper();

    virtual void createDeviceDependentResources();
    virtual void releaseDeviceDependentResources();
    virtual void bindToOffscreenFramebuffer(int index);
    virtual void clearFramebufferWithColor(float r, float g, float b, float a);
    virtual void bindToScreenFramebuffer();
    virtual void useNormalBlending();
    virtual void useScreenBlending();
    virtual void useNoBlending();
    virtual void bindInt4(NGShaderUniformInput* uniform, int4& inValue);
    virtual void bindFloat4(NGShaderUniformInput* uniform, float4& inValue);
    virtual void bindFloat4Array(NGShaderUniformInput* uniform, int count, float4* inValue);
    virtual void bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue);
    virtual void bindMatrix(NGShaderUniformInput* uniform);
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform = NULL);
    virtual void bindNGShader(ShaderProgramWrapper* shaderProgramWrapper);
    virtual void mapTextureVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D_TEXTURE>& vertices);
    virtual void mapBasicVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D>& vertices);
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);

protected:
    virtual TextureWrapper* createFramebuffer();
    virtual void platformReleaseFramebuffers();

private:
    GLint _screenFBO;
    
    std::vector<GLuint> _fbos;
    std::vector<GLuint> _fbo_textures;

    GLuint _textureVertexBuffer;
    GLuint _basicVertexBuffer;
    GLuint _indexBuffer;
    
    template <typename T>
    void mapVertices(GLuint& vertexBuffer, std::vector<T>& vertices, std::vector<NGShaderVarInput*>& inputLayout)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T) * vertices.size(), &vertices[0]);
        
        for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
        {
            NGShaderVarInput* svi = (*i);
            
            glVertexAttribPointer(svi->_attribute, svi->_size, GL_FLOAT, GL_FALSE, svi->_stride, svi->_bufferOffset);
        }
    }
    
    template <typename T>
    void createVertexBuffer(GLuint& vertexBuffer, uint32_t size)
    {
        std::vector<T> vertices;
        vertices.reserve(size);
        
        T vertex;
        for (int i = 0; i < size; ++i)
        {
            vertices.push_back(vertex);
        }
        
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
    }
    
    void createIndexBuffer();
};

#endif /* defined(__noctisgames__OpenGLRendererHelper__) */
