//
//  MainRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MainRenderer__
#define __noctisgames__MainRenderer__

#include "framework/graphics/portable/Renderer.h"

#include "framework/graphics/portable/FontAlign.h"

#include <string>

class NGTexture;
class Font;
struct b2Vec2;
class World;

class MainRenderer : public Renderer
{
public:
    MainRenderer();
    
    virtual ~MainRenderer();
    
    virtual void render(int flags = 0);
    
private:
    Font* _font;
    NGRect* _camBounds;
    
    void renderBackground();
    
    void renderWorld();
    
    void renderEntities(World* world, bool isServer);
    
    void renderAtmosphere();
    
    void renderUI(int engineState);
    
    void renderMainMenuSteamOffText();
    
    void renderMainMenuSteamOnText();
    
    void renderStartingServerText();
    
    void renderEnterUsernameText();
    
    void renderJoiningLocalServerByIPText();
    
    void renderJoiningServerText();
    
    void renderServerJoinedText(int flags);
    
    void renderText(const std::string& inStr, const b2Vec2& origin, const Color& inColor, int justification = FONT_ALIGN_LEFT);
    
    void updateCamera();
};

#endif /* defined(__noctisgames__MainRenderer__) */
