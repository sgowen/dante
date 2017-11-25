//
//  MainEngine.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainEngine__
#define __dante__MainEngine__

#include "framework/main/portable/Engine.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteam.h"
#endif

#include "framework/util/NGRTTI.h"

class Server;
class JsonFile;
class Entity;

class MainEngine : public Engine
{
    NGRTTI_DECL;
    
public:
    MainEngine();
    
    virtual ~MainEngine();
    
    virtual void createDeviceDependentResources();
    
    virtual void releaseDeviceDependentResources();
    
	virtual void update(float deltaTime);
    
protected:
    virtual void onFrame();
    
private:
    JsonFile* _config;
    
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

#endif /* defined(__dante__MainEngine__) */
