//
//  World.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "World.h"

#include "Robot.h"
#include "Entity.h"

World* World::getInstance()
{
    static World instance = World();
    return &instance;
}

void World::addEntityIfPossible(Entity* inEntity)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        World::getInstance()->addRobot(static_cast<Robot*>(inEntity));
    }
}

void World::removeEntityIfPossible(Entity* inEntity)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        World::getInstance()->removeRobot(static_cast<Robot*>(inEntity));
    }
}

void World::addRobot(Robot* inRobot)
{
    m_robots.push_back(inRobot);
    
    int index = static_cast<int>(m_robots.size() - 1);
    inRobot->setIndexInWorld(index);
}

void World::removeRobot(Robot* inRobot)
{
    bool isContained = false;
    int len = static_cast<int>(m_robots.size());
    for (int i = 0; i < len; ++i)
    {
        if (m_robots[i]->getID() == inRobot->getID())
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
            m_robots[index] = m_robots[lastIndex];
            m_robots[index]->setIndexInWorld(index);
        }
        
        inRobot->setIndexInWorld(-1);
        
        m_robots.pop_back();
    }
}

void World::update()
{
    //update all game objects- sometimes they want to die, so we need to tread carefully...
    
    int len = static_cast<int>(m_robots.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Robot* go = m_robots[i];
        
        if (!go->isRequestingDeletion())
        {
            go->update();
        }
        
        //you might suddenly want to die after your update, so check again
        if (go->isRequestingDeletion())
        {
            removeRobot(go);
            go->onDeletion();
            --i;
            --c;
        }
    }
}

const std::vector<Robot*>& World::getRobots() const
{
    return m_robots;
}

World::World()
{
    // Empty
}
