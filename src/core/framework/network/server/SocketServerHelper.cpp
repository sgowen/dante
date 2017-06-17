//
//  SocketServerHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SocketServerHelper.h"

#include "SocketPacketHandler.h"

SocketServerHelper::SocketServerHelper(uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : IServerHelper(new SocketPacketHandler(inPort, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc))
{
    // Empty
}

SocketServerHelper::~SocketServerHelper()
{
    // Empty
}

bool SocketServerHelper::isConnected()
{
    return (static_cast<SocketPacketHandler*>(m_packetHandler))->isInitialized();
}
