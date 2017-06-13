//
//  INGSteamClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__INGSteamClient__
#define __noctisgames__INGSteamClient__

#include "NGSteam.h"

class INGSteamClient
{
public:
    INGSteamClient();
    
    virtual ~INGSteamClient();
    
    // Connect to a server at a given IP address or game server steamID
    virtual void initiateServerConnection(CSteamID steamIDGameServer) = 0;
    virtual void initiateServerConnection(uint32 unServerAddress, const int32 nPort) = 0;
};

#endif /* defined(__noctisgames__INGSteamClient__) */
