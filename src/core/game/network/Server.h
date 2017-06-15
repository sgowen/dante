//
//  Server.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Server__
#define __noctisgames__Server__

class ClientProxy;
class Entity;
class Robot;

class Server
{
public:
    static void create();
    
    static void destroy();
    
    static Server* getInstance();
    
    static void staticHandleNewClient(ClientProxy* inClientProxy);
    
    static void staticHandleLostClient(ClientProxy* inClientProxy);
    
    static void staticAddEntity(Entity* inEntity);
    
    static void staticRemoveEntity(Entity* inEntity);
    
    int run();
    
    void update(float deltaTime);
    
    bool isInitialized();

private:
    static Server* s_pInstance;
    
    float m_fStateTime;
    float m_fFrameStateTime;
    float m_fStateTimeNoEnemies;
    bool m_isInitialized;
    
    void handleNewClient(ClientProxy* inClientProxy);
    
    void handleLostClient(ClientProxy* inClientProxy);
    
    void spawnRobotForPlayer(int inPlayerId);
    
    void respawnEnemiesIfNecessary();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Server();
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /* defined(__noctisgames__Server__) */
