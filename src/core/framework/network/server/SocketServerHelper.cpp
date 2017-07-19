//
//  SocketServerHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SocketServerHelper.h"

#include "SocketAddress.h"
#include "SocketPacketHandler.h"
#include "macros.h"
#include "StringUtil.h"
#include "FrameworkConstants.h"
#include "ClientProxy.h"
#include "OutputMemoryBitStream.h"

SocketServerHelper::SocketServerHelper(uint16_t inPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc) : IServerHelper(new SocketPacketHandler(inPort, inProcessPacketFunc, inHandleNoResponseFunc, inHandleConnectionResetFunc), inGetClientProxyFunc, inHandleClientDisconnectedFunc)
{
    // Empty
}

SocketServerHelper::~SocketServerHelper()
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        ClientProxy* clientProxy = m_getClientProxyFunc(i);
        if (clientProxy)
        {
            SocketAddress* userAddress = static_cast<SocketAddress*>(clientProxy->getMachineAddress());
            
            OutputMemoryBitStream packet;
            packet.write(NW_PACKET_TYPE_SERVER_EXIT);
            
            sendPacket(packet, userAddress);
        }
    }
}

void SocketServerHelper::processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    switch (packetType)
    {
        case NW_PACKET_TYPE_CLIENT_EXIT:
        {
            LOG("Client is leaving the server");
            
            size_t socketAddrHashRemote = (static_cast<SocketAddress*>(inFromAddress))->getHash();
            
            // Find the connection that should exist for this users address
            bool isFound = false;
            for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                ClientProxy* clientProxy = m_getClientProxyFunc(i);
                if (clientProxy)
                {
                    SocketAddress* userAddress = static_cast<SocketAddress*>(clientProxy->getMachineAddress());
                    if (userAddress->getHash() == socketAddrHashRemote)
                    {
                        isFound = true;
                        m_handleClientDisconnectedFunc(clientProxy);
                        break;
                    }
                }
            }
            
            if (!isFound)
            {
                LOG("Got a client leaving server msg, but couldn't find a matching client");
            }
        }
            break;
        default:
            // Socket based Networking doesn't have built-in auth, so there should never be special packets besides client exit
            LOG("Unknown packet type received from %s", inFromAddress->toString().c_str());
            break;
    }
}

void SocketServerHelper::onClientDisconnected(ClientProxy* clientProxy)
{
    UNUSED(clientProxy);
    
    // Nothing to do here.
}

IMachineAddress* SocketServerHelper::getServerAddress()
{
    return (static_cast<SocketPacketHandler*>(m_packetHandler))->getSocketAddress();
}

bool SocketServerHelper::isConnected()
{
    return (static_cast<SocketPacketHandler*>(m_packetHandler))->isInitialized();
}
