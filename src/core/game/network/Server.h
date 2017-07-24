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
    
    static void sHandleNewClient(int playerId, std::string playerName);
    
    static void sHandleLostClient(ClientProxy* inClientProxy, int index);
    
    void update(float deltaTime);
    
    int getPlayerIdForRobotBeingCreated();
    
    void toggleEnemies();
    
    bool isSpawningEnemies();

private:
    static Server* s_instance;
    
    float m_fStateTime;
    float m_fFrameStateTime;
    float m_fStateTimeNoEnemies;
    int m_iPlayerIdForRobotBeingCreated;
    bool m_isSpawningEnemies;
    
    void handleNewClient(int playerId, std::string playerName);
    
    void handleLostClient(ClientProxy* inClientProxy, int index);
    
    void deleteRobotWithPlayerId(uint8_t playerId);
    
    void spawnRobotForPlayer(int inPlayerId, std::string inPlayerName);
    
    void respawnEnemiesIfNecessary();
    
    void spawnCratesIfNecessary();
    
    void clearClientMoves();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Server(bool isSteam);
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /* defined(__noctisgames__Server__) */
