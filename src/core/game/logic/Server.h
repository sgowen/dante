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

class Server
{
public:
    static void create(bool isSteam, uint32_t inNumCratesToSpawn, uint32_t inNumEnemysToSpawn);
    static Server* getInstance();
    static void destroy();
    static void sHandleNewClient(uint8_t playerId, std::string playerName);
    static void sHandleLostClient(ClientProxy* inClientProxy, uint8_t index);
    
    void update();
    uint8_t getPlayerIdForRobotBeingCreated();
    void toggleEnemies();
    void toggleObjects();
    void toggleDisplaying();
    bool isSpawningEnemies();
    bool isSpawningObjects();
    bool isDisplaying();

private:
    static Server* s_instance;
    
    double _stateTime;
    double _frameStateTime;
    double _stateTimeNoEnemies;
    uint8_t _playerIdForRobotBeingCreated;
    uint32_t _numCratesToSpawn;
    uint32_t _numEnemysToSpawn;
    bool _isSpawningEnemies;
    bool _isSpawningObjects;
    bool _isDisplaying;
    bool _hasSpawnedGrounds;
    
    void handleNewClient(uint8_t playerId, std::string playerName);
    void handleLostClient(ClientProxy* inClientProxy, uint8_t index);
    void deleteRobotWithPlayerId(uint8_t playerId);
    void spawnGroundsIfNecessary();
    void spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName);
    void respawnEnemiesIfNecessary();
    void spawnCratesIfNecessary();
    void clearClientMoves();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Server(bool isSteam, uint32_t inNumCratesToSpawn, uint32_t inNumEnemysToSpawn);
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /* defined(__noctisgames__Server__) */
