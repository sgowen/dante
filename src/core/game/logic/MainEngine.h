//
//  MainEngine.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MainEngine__
#define __noctisgames__MainEngine__

#include "framework/main/portable/Engine.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteam.h"
#endif

#include "framework/util/StateMachine.h"

#include "framework/util/NGRTTI.h"

class MainEngineState;
class JsonFile;
class Renderer;

class MainEngine : public Engine
{
    NGRTTI_DECL;
    
public:
    MainEngine();
    virtual ~MainEngine();
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight);
    virtual void releaseDeviceDependentResources();
    
    virtual void onResume();
    virtual void onPause();
    
    virtual void update(double deltaTime);
    virtual void render();
    
private:
    StateMachine<MainEngine, MainEngineState> _stateMachine;
    
    JsonFile* _config;
    Renderer* _renderer;
    
    std::string _serverIPAddress;
    std::string _name;
#ifdef NG_STEAM
    CSteamID _serverSteamID;
#endif
    bool _isSteam;
    
    void handleNonMoveInput();
    
    void activateSteam();
    
    void handleSteamGameServices();
    
    void deactivateSteam();
    
    void startServer();
    
    void joinServer();
    
    void disconnect();
};

#endif /* defined(__noctisgames__MainEngine__) */
