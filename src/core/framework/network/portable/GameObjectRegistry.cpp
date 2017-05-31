//
//  GameObjectRegistry.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "GameObjectRegistry.h"

#include "World.h"

std::unique_ptr<GameObjectRegistry>	GameObjectRegistry::sInstance;

void GameObjectRegistry::StaticInit()
{
    sInstance.reset(new GameObjectRegistry());
}

GameObjectRegistry::GameObjectRegistry()
{
}

void GameObjectRegistry::RegisterCreationFunction(uint32_t inFourCCName, GameObjectCreationFunc inCreationFunction)
{
    mNameToGameObjectCreationFunctionMap[inFourCCName] = inCreationFunction;
}

GameObject* GameObjectRegistry::CreateGameObject(uint32_t inFourCCName)
{
    //no error checking- if the name isn't there, exception!
    GameObjectCreationFunc creationFunc = mNameToGameObjectCreationFunctionMap[inFourCCName];
    
    GameObject* gameObject = creationFunc();
    
    //should the registry depend on the world? this might be a little weird
    //perhaps you should ask the world to spawn things? for now it will be like this
    World::sInstance->AddGameObject(gameObject);
    
    return gameObject;
}
