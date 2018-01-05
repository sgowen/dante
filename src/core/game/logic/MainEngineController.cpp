//
//  MainEngineController.cpp
//  dante
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/MainEngineController.h"

#include "game/logic/TitleEngine.h"
#include "game/logic/GameEngine.h"
#include "game/logic/StudioEngine.h"

NGRTTI_IMPL(MainEngineController, EngineController);

MainEngineController::MainEngineController()
{
    TitleEngine::create();
    GameEngine::create();
    StudioEngine::create();
}

MainEngineController::~MainEngineController()
{
    StudioEngine::destroy();
    GameEngine::destroy();
    TitleEngine::destroy();
}

EngineState* MainEngineController::getInitialState()
{
    return TitleEngine::getInstance();
}
