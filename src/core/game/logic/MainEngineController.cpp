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
#include <framework/entity/EntityMapper.h>
#include <framework/entity/DefaultController.h>
#include <game/logic/BasicFollowAndAttackController.h>
#include <game/logic/DestructibleController.h>
#include <game/logic/PlayerController.h>
#include <framework/util/Config.h>

NGRTTI_IMPL(MainEngineController, EngineController);

MainEngineController::MainEngineController()
{
    TitleEngine::create();
    GameEngine::create();
    StudioEngine::create();
    
    EntityMapper::getInstance()->registerFunction("BasicFollowAndAttackController", BasicFollowAndAttackController::create);
    EntityMapper::getInstance()->registerFunction("DefaultController", DefaultController::create);
    EntityMapper::getInstance()->registerFunction("DestructibleController", DestructibleController::create);
    EntityMapper::getInstance()->registerFunction("PlayerController", PlayerController::create);
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
