//
//  TitleEngine.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TitleEngine__
#define __noctisgames__TitleEngine__

#include "framework/main/portable/EngineState.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteam.h"
#endif

class JsonFile;
class TitleRenderer;

#define TE_MAIN_MENU_STEAM_OFF 1 << 0
#define TE_MAIN_MENU_STEAM_ON 1 << 1
#define TE_MAIN_MENU_STARTING_SERVER 1 << 2
#define TE_MAIN_MENU_ENTERING_USERNAME 1 << 3
#define TE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP 1 << 4

class TitleEngine : public EngineState
{
    NGRTTI_DECL;
    
public:
    static void create();
    static TitleEngine* getInstance();
    static void destroy();
    
    virtual void enter(Engine* engine);
    virtual void update(Engine* engine);
    virtual void exit(Engine* engine);
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight);
    virtual void releaseDeviceDependentResources();
    virtual void onResume();
    virtual void onPause();
    virtual void render();
    
private:
    static TitleEngine* s_instance;
    
    JsonFile* _config;
    TitleRenderer* _renderer;
    std::string _serverIPAddress;
    std::string _name;
#ifdef NG_STEAM
    CSteamID _serverSteamID;
#endif
    bool _isSteam;
    int _state;
    
    bool handleInput(Engine* engine);
    void activateSteam();
    void handleSteamGameServices(Engine* engine);
    void deactivateSteam();
    void startServer();
    void joinServer(Engine* engine);
    void disconnect();
    
    TitleEngine();
    virtual ~TitleEngine();
};

#endif /* defined(__noctisgames__TitleEngine__) */
