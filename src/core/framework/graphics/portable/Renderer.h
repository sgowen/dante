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
class QuadBatcher;
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
class Box2DDebugRenderer;
class b2World;

class Renderer
{
public:
    Renderer();
    
    virtual ~Renderer();
    
    virtual void createDeviceDependentResources();

	virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers);
    
    virtual void releaseDeviceDependentResources();
    
    virtual void render(int flags = 0) = 0;
    
    void onWorldCreated(b2World* world);
    
    void loadTextureDataSync(NGTexture* arg);
    void loadTextureSync(NGTexture* texture);
    void loadTextureAsync(NGTexture* texture);
    void unloadTexture(NGTexture* texture);
    bool ensureTextures();
    bool ensureTexture(NGTexture* texture);
    
protected:
    std::vector<NGTexture*> _textures;
    std::vector<NGTexture *> _loadingTextures;
    std::vector<tthread::thread *> _textureDataLoadingThreads;
    
    RendererHelper* _rendererHelper;
    SpriteBatcher* _spriteBatcher;
    QuadBatcher* _fillQuadBatcher;
    QuadBatcher* _boundsQuadBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    Box2DDebugRenderer* _box2DDebugRenderer;
    TextureLoader* _textureLoader;
    ShaderProgramLoader* _shaderProgramLoader;
    
    ShaderProgram* _textureShaderProgram;
    ShaderProgram* _colorShaderProgram;
    ShaderProgram* _framebufferToScreenShaderProgram;
    
    int _framebufferIndex;
    bool _areDeviceDependentResourcesCreated;
    bool _areWindowSizeDependentResourcesCreated;
    
    void beginFrame();
    
    void setFramebuffer(int framebufferIndex);
    
    void endFrame();
    
    bool isLoadingData();
    
    bool isReadyForRendering();
    
    void renderEntity(Entity &go, TextureRegion& tr, bool flipX = false);
	void renderEntityWithColor(Entity &go, TextureRegion& tr, Color c, bool flipX = false);
    
    void testRenderingSuite();
    
    void handleAsyncTextureLoads();
    
    void cleanUpThreads();
    
    friend void tthreadLoadTextureDataSync(void* arg);
};

#endif /* defined(__noctisgames__Renderer__) */
