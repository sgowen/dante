//
//  NGSteamClientHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamClientHelper__
#define __noctisgames__NGSteamClientHelper__

#include "IClientHelper.h"

#include "IPacketHandler.h"

class NGSteamAddress;

class NGSteamClientHelper : public IClientHelper
{
public:
    NGSteamClientHelper(ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~NGSteamClientHelper();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    virtual std::string& getName();
    
private:
    NGSteamAddress* m_clientSteamID;
    NGSteamAddress* m_serverID;
    std::string m_name;
};

#endif /* defined(__noctisgames__NGSteamClientHelper__) */
