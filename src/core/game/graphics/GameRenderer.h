//
//  GameRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameRenderer__
#define __noctisgames__GameRenderer__

#include "framework/graphics/portable/Renderer.h"

#include "framework/graphics/portable/FontAlign.h"

#include <string>

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
class Color;
class NGRect;
class NGTexture;
class Box2DDebugRenderer;
class Font;
class World;
class NGLightingShader;

#define NUM_SPRITE_BATCHERS 8
#define NUM_CAMERAS 4

class GameRenderer : public Renderer
{
public:
    GameRenderer();
    virtual ~GameRenderer();
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight);
    virtual void releaseDeviceDependentResources();
    virtual void render(int flags = 0);
    
    void updateCamera();
    
private:
    TextureManager* _textureManager;
    RendererHelper* _rendererHelper;
    SpriteBatcher* _spriteBatchers[NUM_SPRITE_BATCHERS];
    PolygonBatcher* _fillPolygonBatcher;
    PolygonBatcher* _boundsPolygonBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    Box2DDebugRenderer* _box2DDebugRenderer;
    NGShaderLoader* _shaderProgramLoader;
    Font* _font;
    NGRect* _camBounds[NUM_CAMERAS];
    NGShader* _textureNGShader;
    NGShader* _colorNGShader;
    NGLightingShader* _lightingNGShader;
    NGShader* _framebufferToScreenNGShader;
    int _fbIndex;
    
    void setFramebuffer(int framebufferIndex, float r = 0, float g = 0, float b = 0, float a = 0);
    void renderWorld(int flags);
    void renderLayers(World* world, bool isNormals);
    void renderEntities(World* world, bool isServer, bool isNormals);
    void renderBox2D();
    void renderUI(int engineState);
    void renderText(const char* inStr, float x, float y, const Color& inColor, int justification = FONT_ALIGN_LEFT);
    void endFrame();
};

#endif /* defined(__noctisgames__GameRenderer__) */
