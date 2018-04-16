//
//  SocketServerHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/server/SocketServerHelper.h>

#include <framework/network/portable/SocketAddress.h>
#include <framework/network/portable/SocketPacketHandler.h>
#include <framework/util/macros.h>
#include <framework/util/StringUtil.h>
#include <framework/util/Constants.h>
#include <framework/network/server/ClientProxy.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/util/InstanceManager.h>

SocketServerHelper::SocketServerHelper(uint16_t inPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc) : ServerHelper(new SocketPacketHandler(static_cast<Timing*>(INSTANCE_MANAGER->getInstance(INSTANCE_TIME_SERVER)), true, inPort, inProcessPacketFunc, inHandleNoResponseFunc, inHandleConnectionResetFunc), inGetClientProxyFunc, inHandleClientDisconnectedFunc)
{
    // Empty
}

SocketServerHelper::~SocketServerHelper()
{
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        ClientProxy* clientProxy = _getClientProxyFunc(i);
        if (clientProxy)
        {
            SocketAddress* userAddress = static_cast<SocketAddress*>(clientProxy->getMachineAddress());
            
            OutputMemoryBitStream packet;
            packet.write(static_cast<uint8_t>(NW_PACKET_TYPE_SERVER_EXIT));
            
            sendPacket(packet, userAddress);
        }
    }
}

void SocketServerHelper::processSpecialPacket(uint8_t packetType, InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    switch (packetType)
    {
        case NW_PACKET_TYPE_CLIENT_EXIT:
        {
            LOG("Client is leaving the server");
            
            size_t socketAddrHashRemote = (static_cast<SocketAddress*>(inFromAddress))->getHash();
            
            // Find the connection that should exist for this users address
            bool isFound = false;
            for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                ClientProxy* clientProxy = _getClientProxyFunc(i);
                if (clientProxy)
                {
                    SocketAddress* userAddress = static_cast<SocketAddress*>(clientProxy->getMachineAddress());
                    if (userAddress->getHash() == socketAddrHashRemote)
                    {
                        isFound = true;
                        _handleClientDisconnectedFunc(clientProxy);
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

MachineAddress* SocketServerHelper::getServerAddress()
{
    return (static_cast<SocketPacketHandler*>(_packetHandler))->getSocketAddress();
}

bool SocketServerHelper::isConnected()
{
    return (static_cast<SocketPacketHandler*>(_packetHandler))->isInitialized();
}
