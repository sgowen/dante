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
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/entity/DefaultController.h>
#include <game/logic/CrawlerController.h>
#include <game/logic/PlayerController.h>
#include <framework/util/Config.h>

NGRTTI_IMPL(MainEngineController, EngineController);

MainEngineController::MainEngineController()
{
    // Empty
}

MainEngineController::~MainEngineController()
{
    // Empty
}

void MainEngineController::init()
{
    NG_CFG->initWithJsonFile("game.cfg");
    EntityMapper::getInstance()->initWithJsonFile("entities.cfg");
    EntityLayoutMapper::getInstance()->initWithJsonFile("maps.cfg");
    
    TitleEngine::create();
    GameEngine::create();
    StudioEngine::create();
    
    EntityMapper::getInstance()->registerFunction("CrawlerController", CrawlerController::create);
    EntityMapper::getInstance()->registerFunction("DefaultController", DefaultController::create);
    EntityMapper::getInstance()->registerFunction("PlayerController", PlayerController::create);
}

void MainEngineController::deinit()
{
    StudioEngine::destroy();
    GameEngine::destroy();
    TitleEngine::destroy();
}

EngineState* MainEngineController::getInitialState()
{
    return TitleEngine::getInstance();
}
