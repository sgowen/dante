//
//  StudioEngine.h
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StudioEngine__
#define __noctisgames__StudioEngine__

#include "framework/main/portable/EngineState.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteam.h"
#endif

class StudioRenderer;
class StudioInputManager;
class World;

enum StudioEngineState
{
    StudioEngineState_Default = 1 << 0
};

class StudioEngine : public EngineState
{
    friend class StudioInputManager;
    friend class StudioRenderer;
    
    NGRTTI_DECL;
    
public:
    static void create();
    static StudioEngine* getInstance();
    static void destroy();
    
    virtual void enter(Engine* engine);
    virtual void update(Engine* engine);
    virtual void exit(Engine* engine);
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight);
    virtual void releaseDeviceDependentResources();
    virtual void onResume();
    virtual void onPause();
    virtual void render(double alpha);
    
private:
    static StudioEngine* s_instance;
    
    StudioRenderer* _renderer;
    StudioInputManager* _input;
    World* _world;
    int _state;
    
    bool handleInput();
    
    StudioEngine();
    virtual ~StudioEngine();
};

#endif /* defined(__noctisgames__StudioEngine__) */
