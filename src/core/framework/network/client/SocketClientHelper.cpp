//
//  SocketClientHelper.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/client/SocketClientHelper.h>

#include <framework/network/portable/SocketAddress.h>

#include <framework/network/portable/SocketPacketHandler.h>
#include <framework/network/portable/SocketAddressFactory.h>
#include <framework/util/macros.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/util/Constants.h>
#include <framework/util/StringUtil.h>
#include <framework/util/InstanceManager.h>

SocketClientHelper::SocketClientHelper(std::string inServerIPAddress, std::string inName, uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) :
ClientHelper(new SocketPacketHandler(static_cast<Timing*>(INSTANCE_MANAGER->getInstance(INSTANCE_TIME_CLIENT)), false, inPort, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)),
_serverAddress(SocketAddressFactory::createIPv4FromString(inServerIPAddress)),
_name(inName)
{
    // Empty
}

SocketClientHelper::~SocketClientHelper()
{
    OutputMemoryBitStream packet;
    packet.write(static_cast<uint8_t>(NW_PACKET_TYPE_CLIENT_EXIT));
    sendPacket(packet);
    
    if (_serverAddress)
    {
        delete _serverAddress;
    }
}

void SocketClientHelper::processSpecialPacket(uint8_t packetType, InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    switch (packetType)
    {
        case NW_PACKET_TYPE_SERVER_EXIT:
            LOG("Server Shutting Down");
            
            if (_serverAddress)
            {
                delete _serverAddress;
                _serverAddress = NULL;
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
    if (_serverAddress)
    {
        NetworkHelper::sendPacket(inOutputStream, _serverAddress);
    }
}

std::string& SocketClientHelper::getName()
{
    return _name;
}

void SocketClientHelper::updateState()
{
    // Socket based Networking doesn't have built-in auth, so we should be ready to say Hello to the server immediately
    _state = _serverAddress ? CLIENT_READY_TO_SAY_HELLO : CLIENT_AUTH_FAILED;
}
