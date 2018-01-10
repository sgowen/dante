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
    static void create(bool isSteam);
    static Server* getInstance();
    static void destroy();
    static void sHandleNewClient(uint8_t playerId, std::string playerName);
    static void sHandleLostClient(ClientProxy* inClientProxy, uint8_t index);
    
    void update();
    uint8_t getPlayerIdForRobotBeingCreated();
    void toggleDisplaying();
    bool isDisplaying();

private:
    static Server* s_instance;
    
    double _stateTime;
    double _frameStateTime;
    uint8_t _playerIdForRobotBeingCreated;
    bool _isDisplaying;
    bool _hasSpawnedObjects;
    
    void handleNewClient(uint8_t playerId, std::string playerName);
    void handleLostClient(ClientProxy* inClientProxy, uint8_t index);
    void deleteRobotWithPlayerId(uint8_t playerId);
    void spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName);
    void spawnObjectsIfNecessary();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Server(bool isSteam);
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /* defined(__noctisgames__Server__) */
