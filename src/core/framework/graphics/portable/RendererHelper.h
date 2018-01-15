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
class NGShaderVarInput;

class RendererHelper
{
public:
    RendererHelper();
    virtual ~RendererHelper();

	virtual void createDeviceDependentResources() = 0;
	virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight);
	virtual void releaseDeviceDependentResources() = 0;
    virtual NGTexture* getFramebuffer(int index) = 0;
    virtual void updateMatrix(float left, float right, float bottom, float top);
    virtual void bindToOffscreenFramebuffer(int index) = 0;
    virtual void clearFramebufferWithColor(float r, float g, float b, float a) = 0;
    virtual void bindToScreenFramebuffer() = 0;
    virtual void useNormalBlending() = 0;
    virtual void useScreenBlending() = 0;
    virtual void useNoBlending() = 0;
    virtual void bindInteger(NGShaderUniformInput* uniform, int inValue) = 0;
    virtual void bindVector2(NGShaderUniformInput* uniform, vec2& inValue) = 0;
    virtual void bindVector3(NGShaderUniformInput* uniform, vec3& inValue) = 0;
    virtual void bindVector4(NGShaderUniformInput* uniform, vec4& inValue) = 0;
    virtual void bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue) = 0;
    virtual void bindMatrix(NGShaderUniformInput* uniform) = 0;
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform = NULL) = 0;
    virtual void bindNGShader(ShaderProgramWrapper* shaderProgramWrapper) = 0;
    virtual void mapScreenVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<SCREEN_VERTEX>& vertices) = 0;
    virtual void unmapScreenVertices();
    virtual void mapTextureVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<TEXTURE_VERTEX>& vertices) = 0;
    virtual void unmapTextureVertices();
    virtual void mapColorVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<COLOR_VERTEX>& vertices) = 0;
    virtual void unmapColorVertices();
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count) = 0;
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count) = 0;
    
    int getRenderWidth();
    int getRenderHeight();
    
protected:
    std::vector<NGTexture *> _framebufferWrappers;
    
    std::vector<TextureWrapper *> _framebuffers;
    std::vector<uint16_t> _indices;
    
    mat4x4 _matrix;
    
    int _screenWidth;
    int _screenHeight;
    int _renderWidth;
    int _renderHeight;
    
    virtual void createFramebufferObject() = 0;
    virtual void releaseFramebuffers() = 0;
    
    void createFramebufferObjects();
    
private:
    void generateIndices(int maxBatchSize);
};

#endif /* defined(__noctisgames__RendererHelper__) */
