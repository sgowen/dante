//
//  NGSteamServerHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamServerHelper__
#define __noctisgames__NGSteamServerHelper__

#include "IServerHelper.h"

#include "IPacketHandler.h"

class NGSteamServerHelper : public IServerHelper
{
public:
    NGSteamServerHelper(ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~NGSteamServerHelper();
};

#endif /* defined(__noctisgames__NGSteamServerHelper__) */
