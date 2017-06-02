//
//  World.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "World.h"

#include "Robot.h"
#include "Entity.h"

std::unique_ptr<World> World::sInstance;

void World::staticInit()
{
    sInstance.reset(new World());
}

void World::addEntityIfPossible(Entity* inEntity)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        World::sInstance->AddRobot((Robot*)inEntity);
    }
}

void World::removeEntityIfPossible(Entity* inEntity)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        World::sInstance->RemoveRobot((Robot*)inEntity);
    }
}

void World::AddRobot(Robot* inRobot)
{
    mRobots.push_back(inRobot);
    
    int index = static_cast<int>(mRobots.size() - 1);
    inRobot->setIndexInWorld(index);
}

void World::RemoveRobot(Robot* inRobot)
{
    bool isContained = false;
    int len = static_cast<int>(mRobots.size());
    for (int i = 0; i < len; ++i)
    {
        if (mRobots[i]->getID() == inRobot->getID())
        {
            isContained = true;
            break;
        }
    }
    
    if (isContained)
    {
        int index = inRobot->getIndexInWorld();
        
        int lastIndex = len - 1;
        
        if (index != lastIndex)
        {
            mRobots[index] = mRobots[lastIndex];
            mRobots[index]->setIndexInWorld(index);
        }
        
        inRobot->setIndexInWorld(-1);
        
        mRobots.pop_back();
    }
}

void World::update()
{
    //update all game objects- sometimes they want to die, so we need to tread carefully...
    
    int len = static_cast<int>(mRobots.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Robot* go = mRobots[i];
        
        if (!go->isRequestingDeletion())
        {
            go->update();
        }
        
        //you might suddenly want to die after your update, so check again
        if (go->isRequestingDeletion())
        {
            RemoveRobot(go);
            go->onDeletion();
            --i;
            --c;
        }
    }
}

World::World()
{
    // Empty
}
