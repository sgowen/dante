//
//  NGSteamServerHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamServerHelper__
#define __noctisgames__NGSteamServerHelper__

#include "IServerHelper.h"

#include "NGSteam.h"
#include "IPacketHandler.h"

class NGSteamAddress;

class NGSteamServerHelper : public IServerHelper
{
public:
    NGSteamServerHelper(const char* inGameDir, const char* inVersionString, const char* inProductName, const char* inGameDescription, uint16 inPort, uint16 inAuthPort, uint16 inUpdaterPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc);
    
    virtual ~NGSteamServerHelper();
    
    virtual void processIncomingPackets();
    
    virtual IMachineAddress* getServerAddress();
    
    virtual bool isConnected();
    
private:
    NGSteamAddress* m_serverSteamAddress;
    std::string m_serverName;
    bool m_isConnectedToSteam; // Track whether our server is connected to Steam ok (meaning we can restrict who plays based on ownership and VAC bans, etc...)
    
    void sendUpdatedServerDetailsToSteam();
};

#endif /* defined(__noctisgames__NGSteamServerHelper__) */
