//
//  NGSteamClientHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamClientHelper.h"

#include "NGSteamGameServices.h"

NGSteamClientHelper::NGSteamClientHelper() : IClientHelper()
{
    if (NG_STEAM_GAME_SERVICES->init() != STEAM_INIT_SUCCESS)
    {
        // TODO
        //m_iRequestedAction = REQUESTED_ACTION_EXIT;
    }
}

NGSteamClientHelper::~NGSteamClientHelper()
{
    NG_STEAM_GAME_SERVICES->deinit();
}
