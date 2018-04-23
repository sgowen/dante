//
//  TitleEngine.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TitleEngine__
#define __noctisgames__TitleEngine__

#include <framework/main/portable/EngineState.h>

class TitleRenderer;

enum TitleEngineState
{
    TitleEngineState_SteamOff =  1 << 0,
    TitleEngineState_SteamOn =   1 << 1,
    TitleEngineState_InputName = 1 << 2,
    TitleEngineState_InputIp =   1 << 3
};

class TitleEngine : public EngineState
{
    friend class TitleRenderer;
    
    DECL_RTTI;
    
public:
    static void create();
    static TitleEngine* getInstance();
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
    static TitleEngine* s_instance;
    
    TitleRenderer* _renderer;
    std::string _serverIPAddress;
    std::string _name;
    bool _isSteam;
    uint32_t _state;
    
    void handleInput(Engine* engine);
    void activateSteam();
    void handleSteamGameServices(Engine* engine);
    void deactivateSteam();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    TitleEngine();
    ~TitleEngine();
    TitleEngine(const TitleEngine&);
    TitleEngine& operator=(const TitleEngine&);
};

#endif /* defined(__noctisgames__TitleEngine__) */
