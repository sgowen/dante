//
//  NGSteamServerHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamServerHelper.h"

#include "NGSteamPacketHandler.h"

#include "NGSteamGameServices.h"

#include <assert.h>

NGSteamServerHelper::NGSteamServerHelper(ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : IServerHelper(new NGSteamPacketHandler(processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc))
{
    assert(NG_STEAM_GAME_SERVICES->init() == STEAM_INIT_SUCCESS);
}

NGSteamServerHelper::~NGSteamServerHelper()
{
    NG_STEAM_GAME_SERVICES->deinit();
}
