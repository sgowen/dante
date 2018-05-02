//
//  StudioEngine.h
//  noctisgames
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StudioEngine__
#define __noctisgames__StudioEngine__

#include <framework/main/portable/EngineState.h>

typedef void (*TestFunc)(Engine* engine, uint32_t& testMap);

class StudioRenderer;
class World;

enum StudioEngineState
{
    SES_DisplayBox2D =           1 <<  0,
    SES_DisplayGrid =            1 <<  1,
    SES_DisplayTypes =           1 <<  2,
    SES_DisplayParallax =        1 <<  3,
    SES_DisplayControls =        1 <<  4,
    SES_DisplayAssets =          1 <<  5,
    SES_DisplayEntities =        1 <<  6,
    SES_DisplayNewMapDialog =    1 <<  7,
    SES_TextInput =              1 <<  8,
    SES_DisplayLoadMapDialog =   1 << 10,
    SES_DisplaySaveMapDialog =   1 << 11,
    SES_DisplaySaveMapAsDialog = 1 << 12,
    SES_Interpolation =          1 << 13,
    
    SES_LayerBitBegin =               14,
    SES_Layer0 =                 1 << 14,
    SES_Layer1 =                 1 << 15,
    SES_Layer2 =                 1 << 16,
    SES_Layer3 =                 1 << 17,
    SES_Layer4 =                 1 << 18,
    SES_Layer5 =                 1 << 19,
    SES_Layer6 =                 1 << 20,
    SES_Layer7 =                 1 << 21,
    SES_Layer8 =                 1 << 22,
    SES_Layer9 =                 1 << 23,
    SES_NumLayers =                   10,
    
    SES_LayerAll = SES_Layer0 | SES_Layer1 | SES_Layer2 | SES_Layer3 | SES_Layer4 | SES_Layer5 | SES_Layer6 | SES_Layer7 | SES_Layer8 | SES_Layer9
};

enum StudioEngineTextInputField
{
    SETIF_WaterDepth = 1,
    SETIF_WaterWidth = 2
};

enum StudioEngineTextInputType
{
    SETIT_Text =    1,
    SETIT_Integer = 2,
};

class StudioEngine : public EngineState
{
    friend class StudioInputManager;
    friend class StudioRenderer;
    
    DECL_RTTI;
    
public:
    static void create(TestFunc testFunc);
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
    World* _world;
    Engine* _engine;
    TestFunc _testFunc;
    uint32_t _state;
    uint32_t _textInputField;
    uint32_t _textInputType;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    StudioEngine(TestFunc testFunc);
    ~StudioEngine();
    StudioEngine(const StudioEngine&);
    StudioEngine& operator=(const StudioEngine&);
};

#endif /* defined(__noctisgames__StudioEngine__) */
