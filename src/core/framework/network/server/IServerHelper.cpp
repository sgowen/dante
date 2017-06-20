//
//  IServerHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "IServerHelper.h"

IServerHelper::IServerHelper(IPacketHandler* packetHandler, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc) : INetworkHelper(packetHandler),
m_getClientProxyFunc(inGetClientProxyFunc),
m_handleClientDisconnectedFunc(inHandleClientDisconnectedFunc)
{
    // Empty
}

IServerHelper::~IServerHelper()
{
    // Empty
}
