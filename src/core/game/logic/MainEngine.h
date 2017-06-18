//
//  MainEngine.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainEngine__
#define __dante__MainEngine__

#include "IEngine.h"

#include "NGSteam.h"

#include "RTTI.h"

class Server;
class JsonFile;
class MainRenderer;
class Vector2;
class Entity;

class MainEngine : public IEngine
{
    RTTI_DECL;
    
public:
    static void staticAddEntity(Entity* inEntity);
    
    static void staticRemoveEntity(Entity* inEntity);
    
    MainEngine();
    
    virtual ~MainEngine();
    
    virtual void createDeviceDependentResources();
    
    virtual void createWindowSizeDependentResources(int renderWidth, int renderHeight, int touchScreenWidth, int touchScreenHeight);
    
    virtual void releaseDeviceDependentResources();

	virtual void onResume();
	
	virtual void onPause();
    
	virtual void update(float deltaTime);
    
    virtual void render();
    
private:
    JsonFile* m_config;
    MainRenderer* m_renderer;
    
    float m_fStateTime;
    float m_fFrameStateTime;
    std::string m_serverIPAddress;
    std::string m_name;
    CSteamID m_serverSteamID;
    int m_iEngineState;
    bool m_isSteam;
    
    void handleNonMoveInput();
    
    void activateSteam();
    
    void deactivateSteam();
    
    void startServer();
    
    void joinServer();
    
    void leaveServer();
};

#endif /* defined(__dante__MainEngine__) */
