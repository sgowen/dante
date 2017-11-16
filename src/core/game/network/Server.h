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
    static void create(bool isSteam, uint32_t inNumCratesToSpawn = 6, uint32_t inNumSpacePiratesToSpawn = 4);
    
    static void destroy();
    
    static Server* getInstance();
    
    static void sHandleNewClient(uint8_t playerId, std::string playerName);
    
    static void sHandleLostClient(ClientProxy* inClientProxy, uint8_t index);
    
    void update(float deltaTime);
    
    uint8_t getPlayerIdForRobotBeingCreated();
    
    void toggleEnemies();
    
    void toggleObjects();
    
    void toggleDisplaying();
    
    bool isSpawningEnemies();
    
    bool isSpawningObjects();
    
    bool isDisplaying();

private:
    static Server* s_instance;
    
    float m_fStateTime;
    float m_fFrameStateTime;
    float m_fStateTimeNoEnemies;
    uint8_t m_iPlayerIdForRobotBeingCreated;
    uint32_t m_iNumCratesToSpawn;
    uint32_t m_iNumSpacePiratesToSpawn;
    bool m_isSpawningEnemies;
    bool m_isSpawningObjects;
    bool m_isDisplaying;
    
    void handleNewClient(uint8_t playerId, std::string playerName);
    
    void handleLostClient(ClientProxy* inClientProxy, uint8_t index);
    
    void deleteRobotWithPlayerId(uint8_t playerId);
    
    void spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName);
    
    void respawnEnemiesIfNecessary();
    
    void spawnCratesIfNecessary();
    
    void clearClientMoves();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Server(bool isSteam, uint32_t inNumCratesToSpawn, uint32_t inNumSpacePiratesToSpawn);
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /* defined(__noctisgames__Server__) */
