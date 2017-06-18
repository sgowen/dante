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

class IClientHelper : public INetworkHelper
{
public:
    IClientHelper(IPacketHandler* packetHandler);
    
    virtual ~IClientHelper();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream) = 0;
    
    virtual std::string& getName() = 0;
    
    virtual bool isConnected() = 0;
};

#endif /* defined(__noctisgames__IClientHelper__) */
