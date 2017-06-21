//
//  MainRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainRenderer__
#define __dante__MainRenderer__

#include "Renderer.h"

#include <string>

class TextureWrapper;
class Font;
class Vector2;

class MainRenderer : public Renderer
{
public:
    MainRenderer(int maxBatchSize);
    
    virtual ~MainRenderer();
    
    virtual void createDeviceDependentResources();
    
    virtual void releaseDeviceDependentResources();
    
    void renderWorld(int engineState);
    
private:
    TextureWrapper* m_characters;
    TextureWrapper* m_misc;
    TextureWrapper* m_bg1;
    TextureWrapper* m_bg2;
    TextureWrapper* m_cover;
    Font* m_font;
    
    void renderMainMenuSteamOffText();
    
    void renderMainMenuSteamOnText();
    
    void renderStartingServerText();
    
    void renderEnterUsernameText();
    
    void renderJoiningLocalServerByIPText();
    
    void renderJoiningServerText();
    
    void renderText(const std::string& inStr, const Vector2& origin, const Color& inColor, bool isLeftJustified = false);
};

#endif /* defined(__dante__MainRenderer__) */
