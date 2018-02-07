//
//  RendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RendererHelper__
#define __noctisgames__RendererHelper__

#include <framework/graphics/portable/NGPrimitiveType.h>
#include <framework/graphics/portable/NGTextureSlot.h>
#include <framework/graphics/portable/VertexProgramInput.h>

#include <vector>

class NGTexture;
struct TextureWrapper;
struct ShaderProgramWrapper;
class NGShaderUniformInput;

class RendererHelper
{
public:
    RendererHelper();
    virtual ~RendererHelper();

	virtual void createDeviceDependentResources() = 0;
    virtual void releaseDeviceDependentResources() = 0;
    virtual void bindToOffscreenFramebuffer(int index) = 0;
    virtual void clearFramebufferWithColor(float r, float g, float b, float a) = 0;
    virtual void bindToScreenFramebuffer() = 0;
    virtual void useNormalBlending() = 0;
    virtual void useScreenBlending() = 0;
    virtual void useNoBlending() = 0;
    virtual void bindInt4(NGShaderUniformInput* uniform, int4& inValue) = 0;
    virtual void bindFloat4(NGShaderUniformInput* uniform, float4& inValue) = 0;
    virtual void bindFloat4Array(NGShaderUniformInput* uniform, int count, float4* inValue) = 0;
    virtual void bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue) = 0;
    virtual void bindMatrix(NGShaderUniformInput* uniform) = 0;
    virtual void bindShader(ShaderProgramWrapper* shaderProgramWrapper) = 0;
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform = NULL) = 0;
    virtual void mapTextureVertices(std::vector<VERTEX_2D_TEXTURE>& vertices) = 0;
    virtual void mapVertices(std::vector<VERTEX_2D>& vertices) = 0;
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count) = 0;
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count) = 0;
    
    void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight);
    void updateMatrix(float left, float right, float bottom, float top);
    NGTexture* getFramebuffer(int index);
    
protected:
    std::vector<NGTexture *> _framebufferWrappers;
    mat4x4 _matrix;
    int _screenWidth;
    int _screenHeight;
    int _renderWidth;
    int _renderHeight;
    
    virtual TextureWrapper* createFramebuffer() = 0;
    virtual void platformReleaseFramebuffers() = 0;
    
    void releaseFramebuffers();
};

#endif /* defined(__noctisgames__RendererHelper__) */
