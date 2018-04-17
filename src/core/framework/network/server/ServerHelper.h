//
//  ServerHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ServerHelper__
#define __noctisgames__ServerHelper__

#include <framework/network/portable/NetworkHelper.h>

class ClientProxy;

typedef ClientProxy* (*GetClientProxyFunc)(uint8_t inPlayerIndex);
typedef void (*HandleClientDisconnectedFunc)(ClientProxy* inClientProxy);

class ServerHelper : public NetworkHelper
{
public:
    ServerHelper(PacketHandler* packetHandler, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc);
    
    virtual ~ServerHelper();
    
    virtual void onClientDisconnected(ClientProxy* clientProxy) = 0;
    
    virtual MachineAddress* getServerAddress() = 0;
    
    virtual bool isConnected() = 0;
    
protected:
    GetClientProxyFunc _getClientProxyFunc;
    HandleClientDisconnectedFunc _handleClientDisconnectedFunc;
};

#endif /* defined(__noctisgames__ServerHelper__) */
