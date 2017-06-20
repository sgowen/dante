//
//  IServerHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__IServerHelper__
#define __noctisgames__IServerHelper__

#include "INetworkHelper.h"

class ClientProxy;

typedef ClientProxy* (*GetClientProxyFunc)(int inPlayerId);
typedef void (*HandleClientDisconnectedFunc)(ClientProxy* inClientProxy);

class IServerHelper : public INetworkHelper
{
public:
    IServerHelper(IPacketHandler* packetHandler, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc);
    
    virtual ~IServerHelper();
    
    virtual void onClientDisconnected(ClientProxy* clientProxy) = 0;
    
    virtual IMachineAddress* getServerAddress() = 0;
    
    virtual bool isConnected() = 0;
    
protected:
    GetClientProxyFunc m_getClientProxyFunc;
    HandleClientDisconnectedFunc m_handleClientDisconnectedFunc;
};

#endif /* defined(__noctisgames__IServerHelper__) */
