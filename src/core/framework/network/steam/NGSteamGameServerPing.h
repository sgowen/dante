//
//  NGSteamGameServerPing.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamGameServerPing__
#define __noctisgames__NGSteamGameServerPing__

#include "NGSteam.h"

class INGSteamClient;

// simple class to marshal callbacks from pinging a game server
class NGSteamGameServerPing : public ISteamMatchmakingPingResponse
{
public:
    NGSteamGameServerPing();
    
    void retrieveSteamIDFromGameServer(INGSteamClient* client, uint32 unIP, uint16 unPort);
    
    void cancelPing();
    
    // Server has responded successfully and has updated data
    virtual void ServerResponded(gameserveritem_t &server);
    
    // Server failed to respond to the ping request
    virtual void ServerFailedToRespond();
    
private:
    HServerQuery m_hGameServerQuery; // we're ping a game server, so we can convert IP:Port to a steamID
    INGSteamClient* m_client;
};

#endif /* defined(__noctisgames__NGSteamGameServerPing__) */
