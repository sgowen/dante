//
//  IClientHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "IClientHelper.h"

IClientHelper::IClientHelper(IPacketHandler* packetHandler) : INetworkHelper(packetHandler), _state(CLIENT_NOT_READY_TO_SAY_HELLO)
{
    // Empty
}

IClientHelper::~IClientHelper()
{
    // Empty
}

int IClientHelper::getState()
{
    return _state;
}
