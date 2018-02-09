//
//  StudioRenderer.h
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StudioRenderer__
#define __noctisgames__StudioRenderer__

#include "framework/graphics/portable/Renderer.h"

#include "framework/graphics/portable/FontAlign.h"

#include <string>
#include <deque>
#include <vector>

class TextureManager;
class SpriteBatcher;
class PolygonBatcher;
class LineBatcher;
class CircleBatcher;
class NGShaderLoader;
class RendererHelper;
class NGShader;
class Entity;
class TextureRegion;
class NGRect;
class NGTexture;
class Font;
class World;
class StudioEngine;
class Box2DDebugRenderer;
class StudioEngine;
class StudioInputManager;

#define NUM_SPRITE_BATCHERS 10
#define NUM_CAMERAS 4

class StudioRenderer : public Renderer
{
public:
    StudioRenderer();
    virtual ~StudioRenderer();
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight);
    virtual void releaseDeviceDependentResources();
    virtual void render();
    
    void setEngine(StudioEngine* inValue);
    void setInputManager(StudioInputManager* inValue);
    
    void update(float x, float y, float w, float h, int scale);
    void displayToast(std::string toast);
    
private:
    TextureManager* _textureManager;
    RendererHelper* _rendererHelper;
    SpriteBatcher* _fontSpriteBatcher;
    SpriteBatcher* _fbSpriteBatcher;
    SpriteBatcher* _spriteBatchers[NUM_SPRITE_BATCHERS];
    std::string _textures[NUM_SPRITE_BATCHERS];
    PolygonBatcher* _fillPolygonBatcher;
    PolygonBatcher* _boundsPolygonBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    Box2DDebugRenderer* _box2DDebugRenderer;
    NGShaderLoader* _shaderProgramLoader;
    Font* _font;
    NGShader* _textureNGShader;
    NGShader* _colorNGShader;
    NGShader* _framebufferToScreenNGShader;
    NGRect* _camBounds[NUM_CAMERAS];
    std::deque<std::string> _toasts;
    int _toastStateTime;
    int _fbIndex;
    int _scrollValue;
    StudioEngine* _engine;
    StudioInputManager* _input;
    uint32_t _engineState;
    NGTexture* _fontTexture;
    
    void setFramebuffer(int framebufferIndex, float r = 0, float g = 0, float b = 0, float a = 0);
    void renderWorld();
    void renderEntities(std::vector<Entity*>& entities);
    void endBatchWithTexture(SpriteBatcher* sb, NGTexture* tex, int layer);
    void renderBox2D();
    void renderGrid();
    void renderUI();
    void renderText(const char* inStr, float x, float y, int justification = FONT_ALIGN_LEFT);
    void endFrame();
};

#endif /* defined(__noctisgames__StudioRenderer__) */
