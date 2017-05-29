//
//  Engine.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Engine.h"

#include "SocketUtil.h"
#include "GameObjectRegistry.h"
#include "World.h"

std::unique_ptr<Engine>	Engine::sInstance;

Engine::Engine()
{
    SocketUtil::StaticInit();
    
    GameObjectRegistry::StaticInit();
    
    World::StaticInit();
}

Engine::~Engine()
{
    SocketUtil::CleanUp();
}

int Engine::Run()
{
    // Override this
    return -2;
}

void Engine::DoFrame()
{
    World::sInstance->Update();
}
