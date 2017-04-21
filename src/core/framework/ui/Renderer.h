//
//  Renderer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Renderer__
#define __noctisgames__Renderer__

#include <vector>
#include <thread>

class SpriteBatcher;
class NGRectBatcher;
class LineBatcher;
class CircleBatcher;
class ITextureLoader;
class IRendererHelper;
class GpuProgramWrapper;
class PhysicalEntity;
class TextureRegion;
struct Color;
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
    
    virtual void beginFrame();
    
    void setFramebuffer(int framebufferIndex);
    
    void renderToScreen();
    
    void endFrame();
    
    bool isLoadingData();

	bool isReadyForRendering();
    
protected:
    SpriteBatcher* m_spriteBatcher;
    NGRectBatcher* m_fillNGRectBatcher;
    NGRectBatcher* m_boundsNGRectBatcher;
    LineBatcher* m_lineBatcher;
    CircleBatcher* m_circleBatcher;
    
    ITextureLoader* m_textureLoader;
    IRendererHelper* m_rendererHelper;
    
    GpuProgramWrapper* m_textureGpuProgramWrapper;
    GpuProgramWrapper* m_colorGpuProgramWrapper;
    GpuProgramWrapper* m_framebufferToScreenGpuProgramWrapper;
    
    int m_iFramebufferIndex;
    
    void renderPhysicalEntity(PhysicalEntity &go, TextureRegion& tr);

	void renderPhysicalEntityWithColor(PhysicalEntity &go, TextureRegion& tr, Color c);
    
    void renderBoundsForPhysicalEntity(PhysicalEntity &pe);
    
    void renderBoundsWithColor(NGRect &r, Color& c);
    
    void renderHighlightForPhysicalEntity(PhysicalEntity &pe, Color &c);
    
    void loadTextureSync(TextureWrapper* textureWrapper);
    
    void loadTextureAsync(TextureWrapper* textureWrapper);
    
    void unloadTexture(TextureWrapper* textureWrapper);
    
    bool ensureTexture(TextureWrapper* textureWrapper);
    
private:
    std::vector<TextureWrapper *> m_loadingTextures;
    std::vector<std::thread *> m_textureDataLoadingThreads;
	int m_iMaxBatchSize;
    bool m_areDeviceDependentResourcesCreated;
	bool m_areWindowSizeDependentResourcesCreated;
    
    void handleAsyncTextureLoads();
    
    void cleanUpThreads();
};

#endif /* defined(__noctisgames__Renderer__) */
