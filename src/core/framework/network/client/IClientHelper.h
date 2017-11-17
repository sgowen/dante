//
//  IClientHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__IClientHelper__
#define __noctisgames__IClientHelper__

#include "INetworkHelper.h"

#include <string>

#define CLIENT_NOT_READY_TO_SAY_HELLO 1
#define CLIENT_READY_TO_SAY_HELLO 2
#define CLIENT_AUTH_FAILED 3

class IClientHelper : public INetworkHelper
{
public:
    IClientHelper(IPacketHandler* packetHandler);
    
    virtual ~IClientHelper();
    
    virtual void handleUninitialized() = 0;
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream) = 0;
    
    virtual std::string& getName() = 0;
    
    int getState();
    
protected:
    int _state;
};

#endif /* defined(__noctisgames__IClientHelper__) */
