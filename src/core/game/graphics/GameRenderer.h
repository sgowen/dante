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
class Box2DDebugRenderer;
class Font;
class World;
class NGLightingShader;
class GameEngine;

#define NUM_SPRITE_BATCHERS 10
#define NUM_CAMERAS 4

struct LightDef
{
    float _lightPosX;
    float _lightPosY;
    float _lightColorR;
    float _lightColorG;
    float _lightColorB;
    float _lightColorA;
    
    LightDef(float lightPosX, float lightPosY, float lightColorR, float lightColorG, float lightColorB, float lightColorA) : _lightPosX(lightPosX), _lightPosY(lightPosY), _lightColorR(lightColorR), _lightColorG(lightColorG), _lightColorB(lightColorB), _lightColorA(lightColorA) {}
};

class GameRenderer : public Renderer
{
public:
    GameRenderer();
    virtual ~GameRenderer();
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight);
    virtual void releaseDeviceDependentResources();
    virtual void render();
    
    void onNewMapLoaded();
    void setEngine(GameEngine* inValue);
    
private:
    TextureManager* _textureManager;
    RendererHelper* _rendererHelper;
    SpriteBatcher* _spriteBatchers[NUM_SPRITE_BATCHERS];
    std::string _textures[NUM_SPRITE_BATCHERS];
    std::string _normals[NUM_SPRITE_BATCHERS];
    SpriteBatcher* _fontSpriteBatcher;
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
    GameEngine* _engine;
    int _fbIndex;
    uint32_t _map;
    uint32_t _engineState;
    NGTexture* _fontTexture;
    std::vector<LightDef> _playerLights;
    std::vector<LightDef> _lights;
    
    void updateCamera();
    void setFramebuffer(int framebufferIndex, float r = 0, float g = 0, float b = 0, float a = 0);
    void renderWorld();
    void renderEntities(std::vector<Entity*>& entities);
    void endBatchWithTexture(SpriteBatcher* sb, NGTexture* tex, int layer);
    void renderBox2D();
    void renderUI();
    void renderText(const char* inStr, float x, float y, int justification = FONT_ALIGN_LEFT);
    void endFrame();
};

#endif /* defined(__noctisgames__GameRenderer__) */
