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
    StudioEngineState_DisplayBox2D =           1 <<  0,
    StudioEngineState_DisplayGrid =            1 <<  1,
    StudioEngineState_DisplayTypes =           1 <<  2,
    StudioEngineState_DisplayParallax =        1 <<  3,
    StudioEngineState_DisplayControls =        1 <<  4,
    StudioEngineState_DisplayAssets =          1 <<  5,
    StudioEngineState_DisplayEntities =        1 <<  6,
    StudioEngineState_DisplayNewMapDialog =    1 <<  7,
    StudioEngineState_TextInput =              1 <<  8,
    StudioEngineState_DisplayLoadMapDialog =   1 << 10,
    StudioEngineState_DisplaySaveMapDialog =   1 << 11,
    StudioEngineState_DisplaySaveMapAsDialog = 1 << 12,
    StudioEngineState_Interpolation =          1 << 13,
    
    StudioEngineState_LayerBitBegin =               14,
    StudioEngineState_Layer0 =                 1 << 14,
    StudioEngineState_Layer1 =                 1 << 15,
    StudioEngineState_Layer2 =                 1 << 16,
    StudioEngineState_Layer3 =                 1 << 17,
    StudioEngineState_Layer4 =                 1 << 18,
    StudioEngineState_Layer5 =                 1 << 19,
    StudioEngineState_Layer6 =                 1 << 20,
    StudioEngineState_Layer7 =                 1 << 21,
    StudioEngineState_Layer8 =                 1 << 22,
    StudioEngineState_Layer9 =                 1 << 23,
    StudioEngineState_NumLayers =                   10,
    
    StudioEngineState_LayerAll = StudioEngineState_Layer0 | StudioEngineState_Layer1 | StudioEngineState_Layer2 | StudioEngineState_Layer3 | StudioEngineState_Layer4 | StudioEngineState_Layer5 | StudioEngineState_Layer6 | StudioEngineState_Layer7 | StudioEngineState_Layer8 | StudioEngineState_Layer9
};

enum StudioEngineTextInputField
{
    StudioEngineTextInputField_WaterDepth = 1,
    StudioEngineTextInputField_WaterWidth = 2
};

enum StudioEngineTextInputType
{
    StudioEngineTextInputType_Text =    1,
    StudioEngineTextInputType_Integer = 2,
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
