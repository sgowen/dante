//
//  World.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "World.h"

#include "RoboCat.h"

std::unique_ptr<World> World::sInstance;

void World::StaticInit()
{
    sInstance.reset(new World());
}

World::World()
{
    // Empty
}

void World::AddRoboCat(RoboCat* inRoboCat)
{
    mRoboCats.push_back(inRoboCat);
    
    int index = static_cast<int>(mRoboCats.size() - 1);
    inRoboCat->setIndexInWorld(index);
}

void World::RemoveRoboCat(RoboCat* inRoboCat)
{
    bool isContained = false;
    int len = static_cast<int>(mRoboCats.size());
    for (int i = 0; i < len; ++i)
    {
        if (mRoboCats[i]->getID() == inRoboCat->getID())
        {
            isContained = true;
            break;
        }
    }
    
    if (isContained)
    {
        int index = inRoboCat->getIndexInWorld();
        
        int lastIndex = len - 1;
        
        if (index != lastIndex)
        {
            mRoboCats[index] = mRoboCats[lastIndex];
            mRoboCats[index]->setIndexInWorld(index);
        }
        
        inRoboCat->setIndexInWorld(-1);
        
        mRoboCats.pop_back();
    }
}

void World::update()
{
    //update all game objects- sometimes they want to die, so we need to tread carefully...
    
    int len = static_cast<int>(mRoboCats.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        RoboCat* go = mRoboCats[i];
        
        if (!go->isRequestingDeletion())
        {
            go->update();
        }
        
        //you might suddenly want to die after your update, so check again
        if (go->isRequestingDeletion())
        {
            RemoveRoboCat(go);
            go->onDeletion();
            --i;
            --c;
        }
    }
}
