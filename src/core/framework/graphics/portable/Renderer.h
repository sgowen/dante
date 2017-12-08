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
class RendererHelper;
class GpuProgramWrapper;
class Entity;
class TextureRegion;
class Color;
class NGRect;
class TextureWrapper;

class Renderer
{
public:
    Renderer(int maxBatchSize);
    
    virtual ~Renderer();
    
    virtual void createDeviceDependentResources();

	virtual void createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers);
    
    virtual void releaseDeviceDependentResources();
    
    virtual void render(int flags = 0) = 0;
    
    void loadTextureSync(TextureWrapper* textureWrapper);
    
    void loadTextureAsync(TextureWrapper* textureWrapper);
    
    void unloadTexture(TextureWrapper* textureWrapper);
    
    bool ensureTexture(TextureWrapper* textureWrapper);
    
    void loadTextureDataSync(TextureWrapper* arg);
    
    TextureLoader* _textureLoader;
    
protected:
    SpriteBatcher* _spriteBatcher;
    NGRectBatcher* _fillNGRectBatcher;
    NGRectBatcher* _boundsNGRectBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    
    
    RendererHelper* _rendererHelper;
    
    GpuProgramWrapper* _textureGpuProgramWrapper;
    GpuProgramWrapper* _colorGpuProgramWrapper;
    GpuProgramWrapper* _framebufferToScreenGpuProgramWrapper;
    
    int _framebufferIndex;
    
    void beginFrame();
    
    void setFramebuffer(int framebufferIndex);
    
    void renderToScreen();
    
    void endFrame();
    
    bool _isLoadingData();
    
    bool isReadyForRendering();
    
    void renderEntity(Entity &go, TextureRegion& tr, bool flipX = false);

	void renderEntityWithColor(Entity &go, TextureRegion& tr, Color c, bool flipX = false);
    
private:
    std::vector<TextureWrapper *> _loadingTextures;
    std::vector<tthread::thread *> _textureDataLoadingThreads;
	int _maxBatchSize;
    bool _areDeviceDependentResourcesCreated;
	bool _areWindowSizeDependentResourcesCreated;
    
    void handleAsyncTextureLoads();
    
    void cleanUpThreads();
    
    friend void tthreadLoadTextureDataSync(void* arg);
};

#endif /* defined(__noctisgames__Renderer__) */
