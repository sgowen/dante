//
//  IClientHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "IClientHelper.h"

IClientHelper::IClientHelper(IPacketHandler* packetHandler) : INetworkHelper(packetHandler)
{
    // Empty
}

IClientHelper::~IClientHelper()
{
    // Empty
}
