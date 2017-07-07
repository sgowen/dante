//
//  Server.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Server__
#define __noctisgames__Server__

#include <string>

class ClientProxy;
class Entity;
class Robot;

class Server
{
public:
    static void create(bool isSteam);
    
    static void destroy();
    
    static Server* getInstance();
    
    static void staticHandleNewClient(int playerId, std::string playerName);
    
    static void staticHandleLostClient(ClientProxy* inClientProxy, int index);
    
    void update(float deltaTime);
    
    void toggleEnemies();
    
    bool isSpawningEnemies();

private:
    static Server* s_instance;
    
    float m_fStateTime;
    float m_fFrameStateTime;
    float m_fStateTimeNoEnemies;
    bool m_isSpawningEnemies;
    
    void handleNewClient(int playerId, std::string playerName);
    
    void handleLostClient(ClientProxy* inClientProxy, int index);
    
    void spawnRobotForPlayer(int inPlayerId, std::string inPlayerName);
    
    void respawnEnemiesIfNecessary();
    
    void clearClientMoves();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Server(bool isSteam);
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /* defined(__noctisgames__Server__) */
