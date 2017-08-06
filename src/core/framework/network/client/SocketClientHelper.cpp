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
#include "macros.h"
#include "OutputMemoryBitStream.h"
#include "FrameworkConstants.h"
#include "StringUtil.h"

SocketClientHelper::SocketClientHelper(std::string inServerIPAddress, std::string inName, uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) :
IClientHelper(new SocketPacketHandler(inPort, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)),
m_serverAddress(SocketAddressFactory::createIPv4FromString(inServerIPAddress)),
m_name(inName)
{
    // Empty
}

SocketClientHelper::~SocketClientHelper()
{
    OutputMemoryBitStream packet;
    packet.write(NW_PACKET_TYPE_CLIENT_EXIT);
    sendPacket(packet);
    
    if (m_serverAddress)
    {
        delete m_serverAddress;
    }
}

void SocketClientHelper::processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    switch (packetType)
    {
        case NW_PACKET_TYPE_SERVER_EXIT:
            LOG("Server Shutting Down");
            
            if (m_serverAddress)
            {
                delete m_serverAddress;
                m_serverAddress = nullptr;
            }
            
            updateState();
        default:
            // Socket based Networking doesn't have built-in auth, so there should never be special packets besides server exit
            break;
    }
}

void SocketClientHelper::handleUninitialized()
{
    updateState();
}

void SocketClientHelper::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    if (m_serverAddress)
    {
        INetworkHelper::sendPacket(inOutputStream, m_serverAddress);
    }
}

std::string& SocketClientHelper::getName()
{
    return m_name;
}

void SocketClientHelper::updateState()
{
    // Socket based Networking doesn't have built-in auth, so we should be ready to say Hello to the server immediately
    m_iState = m_serverAddress ? CLIENT_READY_TO_SAY_HELLO : CLIENT_AUTH_FAILED;
}
