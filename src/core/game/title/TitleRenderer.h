//
//  TitleRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TitleRenderer__
#define __noctisgames__TitleRenderer__

#include <framework/graphics/portable/Renderer.h>

#include <string>

class TextureManager;
class SpriteBatcher;
class PolygonBatcher;
class LineBatcher;
class CircleBatcher;
class ShaderProgramLoader;
class RendererHelper;
class NGShader;
class Entity;
class TextureRegion;
class NGRect;
class NGTexture;
class Font;
class TitleEngine;

class TitleRenderer : public Renderer
{
public:
    TitleRenderer();
    virtual ~TitleRenderer();
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight);
    virtual void releaseDeviceDependentResources();
    virtual void render();
    
    void setEngine(TitleEngine* inValue);
    
private:
    TextureManager* _textureManager;
    RendererHelper* _rendererHelper;
    SpriteBatcher* _spriteBatcher;
    PolygonBatcher* _fillPolygonBatcher;
    PolygonBatcher* _boundsPolygonBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    ShaderProgramLoader* _shaderProgramLoader;
    Font* _font;
    NGShader* _textureNGShader;
    NGShader* _colorNGShader;
    NGShader* _framebufferToScreenNGShader;
    int _fbIndex;
    TitleEngine* _engine;
    uint32_t _engineState;
    
    void setFramebuffer(int framebufferIndex, float r = 0, float g = 0, float b = 0, float a = 0);
    void renderMainMenuSteamOffText();
    void renderMainMenuSteamOnText();
    void renderStartingServerText();
    void renderEnterUsernameText();
    void renderJoiningLocalServerByIPText();
    void renderText(const char* inStr, float x, float y, int justification);
    void testRenderingSuite();
    void endFrame();
};

#endif /* defined(__noctisgames__TitleRenderer__) */
