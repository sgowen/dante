//
//  StudioRenderer.h
//  noctisgames
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StudioRenderer__
#define __noctisgames__StudioRenderer__

#include <framework/graphics/portable/Renderer.h>

#include <string>
#include <deque>
#include <vector>
#include <map>

class TextureManager;
class SpriteBatcher;
class PolygonBatcher;
class LineBatcher;
class CircleBatcher;
class ShaderProgramLoader;
class RendererHelper;
class NGShader;
class NGWaterShader;
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
struct FramebufferWrapper;

#define NUM_SPRITE_BATCHERS 10
#define NUM_CAMERAS 4

class StudioRenderer : public Renderer
{
public:
    StudioRenderer();
    virtual ~StudioRenderer();
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight);
    virtual void releaseDeviceDependentResources();
    virtual void render();
    
    void setEngine(StudioEngine* inValue);
    void setInputManager(StudioInputManager* inValue);
    void onMapLoaded();
    void onWaterAdded(Entity* e);
    void onWaterRemoved(Entity* e);
    
    void update(float x, float y, float w, float h, int scale);
    void displayToast(std::string toast);
    
private:
    TextureManager* _textureManager;
    RendererHelper* _rendererHelper;
    SpriteBatcher* _fontSpriteBatcher;
    SpriteBatcher* _fbSpriteBatcher;
    SpriteBatcher* _activeEntitySpriteBatcher;
    SpriteBatcher* _spriteBatchers[NUM_SPRITE_BATCHERS];
    std::string _textures[NUM_SPRITE_BATCHERS];
    PolygonBatcher* _fillPolygonBatcher;
    PolygonBatcher* _boundsPolygonBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    Box2DDebugRenderer* _box2DDebugRenderer;
    ShaderProgramLoader* _shaderProgramLoader;
    Font* _font;
    NGShader* _textureShader;
    NGWaterShader* _waterShader;
    NGShader* _colorShader;
    NGShader* _framebufferToScreenShader;
    NGRect* _camBounds[NUM_CAMERAS];
    std::deque<std::string> _toasts;
    int _toastStateTime;
    int _fbIndex;
    int _scrollValue;
    StudioEngine* _engine;
    StudioInputManager* _input;
    uint32_t _engineState;
    uint32_t _textInputField;
    NGTexture* _fontTexture;
    
    void createFramebuffers();
    void bindOffscreenFramebuffer(int fbIndex, float r = 0, float g = 0, float b = 0, float a = 0);
    void bindFramebuffer(FramebufferWrapper* fb, float r = 0, float g = 0, float b = 0, float a = 0);
    void renderWorld();
    void renderEntities(std::vector<Entity*>& entities);
    void renderWater(std::vector<Entity*>& entities);
    void endBatchWithTexture(SpriteBatcher* sb, NGTexture* tex, int layer);
    void renderBox2D();
    void renderGrid();
    void renderUI();
    void renderText(const char* inStr, float x, float y, int justification);
    void endFrame();
};

#endif /* defined(__noctisgames__StudioRenderer__) */
