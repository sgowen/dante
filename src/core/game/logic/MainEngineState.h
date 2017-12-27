//
//  MainEngineState.h
//  dante
//
//  Created by Stephen Gowen on 12/27/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MainEngineState__
#define __noctisgames__MainEngineState__

#define MES_MAIN_MENU_STEAM_OFF 1 << 0
#define MES_MAIN_MENU_STEAM_ON 1 << 1
#define MES_MAIN_MENU_STARTING_SERVER 1 << 2
#define MES_MAIN_MENU_ENTERING_USERNAME 1 << 3
#define MES_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP 1 << 4
#define MES_STEAM_JOINING_SERVER 1 << 5
#define MES_DISPLAY_BOX_2D 1 << 6

#include <framework/util/State.h>
#include <game/logic/MainEngine.h>

class MainEngineState : public State<MainEngine>
{
    virtual void enter(MainEngine* engine) = 0;
    
    virtual void execute(MainEngine* engine) = 0;
    
    virtual void exit(MainEngine* engine) = 0;
    
    virtual void createDeviceDependentResources() = 0;
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight) = 0;
    virtual void releaseDeviceDependentResources() = 0;
    
    virtual void onResume() = 0;
    virtual void onPause() = 0;
    
    virtual void update(double deltaTime) = 0;
    virtual void render() = 0;
};

#endif /* defined(__noctisgames__MainEngineState__) */
