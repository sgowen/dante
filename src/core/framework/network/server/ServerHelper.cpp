//
//  ServerHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/network/server/ServerHelper.h"

ServerHelper::ServerHelper(PacketHandler* packetHandler, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc) : NetworkHelper(packetHandler),
_getClientProxyFunc(inGetClientProxyFunc),
_handleClientDisconnectedFunc(inHandleClientDisconnectedFunc)
{
    // Empty
}

ServerHelper::~ServerHelper()
{
    // Empty
}
