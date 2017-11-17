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
_serverAddress(SocketAddressFactory::createIPv4FromString(inServerIPAddress)),
_name(inName)
{
    // Empty
}

SocketClientHelper::~SocketClientHelper()
{
    OutputMemoryBitStream packet;
    packet.write(NW_PACKET_TYPE_CLIENT_EXIT);
    sendPacket(packet);
    
    if (_serverAddress)
    {
        delete _serverAddress;
    }
}

void SocketClientHelper::processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    switch (packetType)
    {
        case NW_PACKET_TYPE_SERVER_EXIT:
            LOG("Server Shutting Down");
            
            if (_serverAddress)
            {
                delete _serverAddress;
                _serverAddress = nullptr;
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
        INetworkHelper::sendPacket(inOutputStream, _serverAddress);
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
