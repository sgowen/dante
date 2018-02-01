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
    StudioEngineState_Default =                1 <<  0,
    StudioEngineState_DisplayBox2D =           1 <<  1,
    StudioEngineState_DisplayGrid =            1 <<  2,
    StudioEngineState_DisplayTypes =           1 <<  3,
    StudioEngineState_DisplayParallax =        1 <<  4,
    StudioEngineState_DisplayControls =        1 <<  5,
    StudioEngineState_DisplayAssets =          1 <<  6,
    StudioEngineState_DisplayEntities =        1 <<  7,
    StudioEngineState_DisplayNewMapDialog =    1 <<  8,
    StudioEngineState_DisplayLoadMapDialog =   1 <<  9,
    StudioEngineState_DisplaySaveMapDialog =   1 << 10,
    StudioEngineState_DisplaySaveMapAsDialog = 1 << 11,
    StudioEngineState_LayerBitBegin =               12,
    StudioEngineState_Layer0 =                 1 << 12,
    StudioEngineState_Layer1 =                 1 << 13,
    StudioEngineState_Layer2 =                 1 << 14,
    StudioEngineState_Layer3 =                 1 << 15,
    StudioEngineState_Layer4 =                 1 << 16,
    StudioEngineState_Layer5 =                 1 << 17,
    StudioEngineState_Layer6 =                 1 << 18,
    StudioEngineState_Layer7 =                 1 << 19,
    StudioEngineState_Layer8 =                 1 << 20,
    StudioEngineState_Layer9 =                 1 << 21,
    StudioEngineState_NumLayers =                   10,
    
    StudioEngineState_LayerAll = StudioEngineState_Layer0 | StudioEngineState_Layer1 | StudioEngineState_Layer2 | StudioEngineState_Layer3 | StudioEngineState_Layer4 | StudioEngineState_Layer5 | StudioEngineState_Layer6 | StudioEngineState_Layer7 | StudioEngineState_Layer8 | StudioEngineState_Layer9
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
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight);
    virtual void releaseDeviceDependentResources();
    virtual void onResume();
    virtual void onPause();
    virtual void render(double alpha);
    
private:
    static StudioEngine* s_instance;
    
    StudioRenderer* _renderer;
    StudioInputManager* _input;
    World* _world;
    uint32_t _state;
    
    bool handleInput();
    
    StudioEngine();
    virtual ~StudioEngine();
};

#endif /* defined(__noctisgames__StudioEngine__) */
