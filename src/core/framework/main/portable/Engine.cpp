//
//  Engine.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/main/portable/Engine.h"

#include "framework/util/FrameworkConstants.h"

NGRTTI_IMPL_NOPARENT(Engine);

Engine::Engine() :
_stateTime(0),
_frameStateTime(0),
_requestedAction(REQUESTED_ACTION_UPDATE),
_engineState(0)
{
    // Empty
}

Engine::~Engine()
{
    // Empty
}

int Engine::getRequestedAction()
{
    return _requestedAction;
}

void Engine::clearRequestedAction()
{
    _requestedAction = REQUESTED_ACTION_UPDATE;
}
