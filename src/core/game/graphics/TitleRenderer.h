//
//  TitleRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TitleRenderer__
#define __noctisgames__TitleRenderer__

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
class Font;
class World;

class TitleRenderer : public Renderer
{
public:
    TitleRenderer();
    virtual ~TitleRenderer();
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight);
    virtual void releaseDeviceDependentResources();
    virtual void render(int flags = 0);
    
private:
    TextureManager* _textureManager;
    RendererHelper* _rendererHelper;
    SpriteBatcher* _spriteBatcher;
    PolygonBatcher* _fillPolygonBatcher;
    PolygonBatcher* _boundsPolygonBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    NGShaderLoader* _shaderProgramLoader;
    Font* _font;
    NGShader* _textureNGShader;
    NGShader* _colorNGShader;
    NGShader* _framebufferToScreenNGShader;
    int _fbIndex;
    
    void setFramebuffer(int framebufferIndex, float r = 0, float g = 0, float b = 0, float a = 0);
    void renderMainMenuSteamOffText();
    void renderMainMenuSteamOnText();
    void renderStartingServerText();
    void renderEnterUsernameText();
    void renderJoiningLocalServerByIPText();
    void renderText(const char* inStr, float x, float y, const Color& inColor, int justification = FONT_ALIGN_LEFT);
    void testRenderingSuite();
    void endFrame();
};

#endif /* defined(__noctisgames__TitleRenderer__) */
