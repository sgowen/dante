//
//  SocketClientHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SocketClientHelper.h"

#include "SocketAddress.h"

#include "SocketPacketHandler.h"
#include "SocketAddressFactory.h"

SocketClientHelper::SocketClientHelper(const std::string& inServerIPAddress, const std::string& inName, uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) :
IClientHelper(new SocketPacketHandler(inPort, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)),
m_serverAddress(SocketAddressFactory::createIPv4FromString(inServerIPAddress)),
m_name(inName)
{
    // Empty
}

SocketClientHelper::~SocketClientHelper()
{
    delete m_serverAddress;
}

void SocketClientHelper::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    INetworkHelper::sendPacket(inOutputStream, m_serverAddress);
}

std::string& SocketClientHelper::getName()
{
    return m_name;
}
