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
    virtual NGTexture* getFramebuffer(int index);
    virtual void bindToOffscreenFramebuffer(int index);
    virtual void clearFramebufferWithColor(float r, float g, float b, float a);
    virtual void bindToScreenFramebuffer();
    virtual void useNormalBlending();
    virtual void useScreenBlending();
    virtual void useNoBlending();
    virtual void bindMatrix(NGShaderUniformInput* uniform);
    virtual void bindVector2(NGShaderUniformInput* uniform, Vector2& inValue);
    virtual void bindVector3(NGShaderUniformInput* uniform, Vector3& inValue);
    virtual void bindColor(NGShaderUniformInput* uniform, Color& inValue);
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform = NULL);
    virtual void bindNGShader(ShaderProgramWrapper* shaderProgramWrapper);
    virtual void mapScreenVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<SCREEN_VERTEX>& vertices);
    virtual void unmapScreenVertices();
    virtual void mapTextureVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<TEXTURE_VERTEX>& vertices);
    virtual void unmapTextureVertices();
    virtual void mapColorVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<COLOR_VERTEX>& vertices);
    virtual void unmapColorVertices();
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count);

protected:
    virtual void createFramebufferObject();
    virtual void releaseFramebuffers();

private:
    std::vector<GLuint> _fbos;
    std::vector<GLuint> _fbo_textures;

    GLuint _screenVboObject; // For Screen
    GLuint _textureVboObject; // For Sprite Batcher
    GLuint _colorVboObject; // For Geometry Batcher

    GLint _screenFBO;
    GLint _maxTextureSize;
    
    template <typename T>
    void mapVertices(GLuint& vertexBuffer, std::vector<T>& vertices, std::vector<NGShaderVarInput*>& inputLayout)
    {
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        
        for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
        {
            NGShaderVarInput* svi = (*i);
            
            glVertexAttribPointer(svi->_attribute, svi->_size, GL_FLOAT, GL_FALSE, svi->_stride, svi->_bufferOffset);
        }
    }
    
    void unmapVertices(GLuint& vertexBuffer);
};

#endif /* defined(__noctisgames__OpenGLRendererHelper__) */
