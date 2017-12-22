//
//  Renderer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Renderer__
#define __noctisgames__Renderer__

#include <tinythread.h>

#include <vector>

class SpriteBatcher;
class NGRectBatcher;
class LineBatcher;
class CircleBatcher;
class TextureLoader;
class ShaderProgramLoader;
class RendererHelper;
class ShaderProgram;
class Entity;
class TextureRegion;
class Color;
class NGRect;
class NGTexture;

class Renderer
{
public:
    Renderer();
    
    virtual ~Renderer();
    
    virtual void createDeviceDependentResources();

	virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers);
    
    virtual void releaseDeviceDependentResources();
    
    virtual void render(int flags = 0) = 0;
    
    void loadTextureDataSync(NGTexture* arg);
    void loadTextureSync(NGTexture* texture);
    void loadTextureAsync(NGTexture* texture);
    void unloadTexture(NGTexture* texture);
    bool ensureTextures();
    bool ensureTexture(NGTexture* texture);
    
protected:
    RendererHelper* _rendererHelper;
    SpriteBatcher* _spriteBatcher;
    NGRectBatcher* _fillNGRectBatcher;
    NGRectBatcher* _boundsNGRectBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    TextureLoader* _textureLoader;
    ShaderProgramLoader* _shaderProgramLoader;
    
    ShaderProgram* _textureShaderProgram;
    ShaderProgram* _colorShaderProgram;
    ShaderProgram* _framebufferToScreenShaderProgram;
    
    std::vector<NGTexture*> _textures;
    
    int _framebufferIndex;
    
    void beginFrame();
    
    void setFramebuffer(int framebufferIndex);
    
    void endFrame();
    
    bool isLoadingData();
    
    bool isReadyForRendering();
    
    void renderEntity(Entity &go, TextureRegion& tr, bool flipX = false);
	void renderEntityWithColor(Entity &go, TextureRegion& tr, Color c, bool flipX = false);
    
    void testRenderingSuite();
    
private:
    std::vector<NGTexture *> _loadingTextures;
    std::vector<tthread::thread *> _textureDataLoadingThreads;
	int _maxBatchSize;
    bool _areDeviceDependentResourcesCreated;
	bool _areWindowSizeDependentResourcesCreated;
    
    void handleAsyncTextureLoads();
    
    void cleanUpThreads();
    
    friend void tthreadLoadTextureDataSync(void* arg);
};

#endif /* defined(__noctisgames__Renderer__) */
