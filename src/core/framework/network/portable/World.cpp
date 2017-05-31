//
//  World.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "World.h"

std::unique_ptr<World> World::sInstance;

void World::StaticInit()
{
    sInstance.reset(new World());
}

World::World()
{
    // Empty
}

void World::AddGameObject(GameObject* inGameObject)
{
    mGameObjects.push_back(inGameObject);
    int index = static_cast<int>(mGameObjects.size() - 1);
    inGameObject->SetIndexInWorld(index);
}

void World::RemoveGameObject(GameObject* inGameObject)
{
    bool isContained = false;
    int len = static_cast<int>(mGameObjects.size());
    for (int i = 0; i < len; ++i)
    {
        if (mGameObjects[i]->GetNetworkId() == inGameObject->GetNetworkId())
        {
            isContained = true;
            break;
        }
    }
    
    if (isContained)
    {
        int index = inGameObject->GetIndexInWorld();
        
        int lastIndex = len - 1;
        
        if (index != lastIndex)
        {
            mGameObjects[index] = mGameObjects[lastIndex];
            mGameObjects[index]->SetIndexInWorld(index);
        }
        
        inGameObject->SetIndexInWorld(-1);
        
        mGameObjects.pop_back();
    }
}

void World::Update()
{
    //update all game objects- sometimes they want to die, so we need to tread carefully...
    
    int len = static_cast<int>(mGameObjects.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        GameObject* go = mGameObjects[i];
        
        if (!go->DoesWantToDie())
        {
            go->Update();
        }
        
        //you might suddenly want to die after your update, so check again
        if (go->DoesWantToDie())
        {
            RemoveGameObject(go);
            go->HandleDying();
            --i;
            --c;
        }
    }
}
