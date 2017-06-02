//
//  Server.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Server__
#define __noctisgames__Server__

class ClientProxy;
class Robot;

class Server
{
public:
    static Server* getInstance();
    
    static void staticHandleNewClient(ClientProxy* inClientProxy);
    static void staticHandleLostClient(ClientProxy* inClientProxy);
    
    int run();
    
    void handleNewClient(ClientProxy* inClientProxy);
    void handleLostClient(ClientProxy* inClientProxy);
    
    void spawnCatForPlayer(int inPlayerId);
    
    bool isInitialized();

private:
    float m_fFrameStateTime;
    bool m_isInitialized;
    
    Robot* getCatForPlayer(int inPlayerId);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Server();
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /* defined(__noctisgames__Server__) */
