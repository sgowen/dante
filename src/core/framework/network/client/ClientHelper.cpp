//
//  ClientHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ClientHelper.h"

ClientHelper::ClientHelper(PacketHandler* packetHandler) : NetworkHelper(packetHandler), _state(CLIENT_NOT_READY_TO_SAY_HELLO)
{
    // Empty
}

ClientHelper::~ClientHelper()
{
    // Empty
}

int ClientHelper::getState()
{
    return _state;
}
