//
//  World.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "World.h"

#include "Entity.h"
#include "Robot.h"
#include "SpacePirate.h"

World::World()
{
    // Empty
}

void World::addEntity(Entity* inEntity)
{
    m_entities.push_back(inEntity);
}

void World::removeEntity(Entity* inEntity)
{
    int len = static_cast<int>(m_entities.size());
    int indexToRemove = -1;
    for (int i = 0; i < len; ++i)
    {
        if (m_entities[i]->getID() == inEntity->getID())
        {
            indexToRemove = i;
            break;
        }
    }
    
    if (indexToRemove > -1)
    {
        int lastIndex = len - 1;
        
        if (indexToRemove != lastIndex)
        {
            m_entities[indexToRemove] = m_entities[lastIndex];
        }
        
        m_entities.pop_back();
    }
}

void World::update()
{
    //update all game objects- sometimes they want to die, so we need to tread carefully...
    
    int len = static_cast<int>(m_entities.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* go = m_entities[i];
        
        if (!go->isRequestingDeletion())
        {
            go->update();
        }
        
        //you might suddenly want to die after your update, so check again
        if (go->isRequestingDeletion())
        {
            removeEntity(go);
            go->onDeletion();
            --i;
            --c;
        }
    }
}

Robot* World::getRobotWithPlayerId(int inPlayerID)
{
    for (Entity* entity : m_entities)
    {
        Robot* robot = entity->getRTTI().derivesFrom(Robot::rtti) ? static_cast<Robot*>(entity) : nullptr;
        if (robot && robot->getPlayerId() == inPlayerID)
        {
            return robot;
        }
    }
    
    return nullptr;
}

bool World::hasSpacePirates()
{
    for (Entity* entity : m_entities)
    {
        if (entity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            return true;
        }
    }
    
    return false;
}

std::vector<Entity*>& World::getEntities()
{
    return m_entities;
}
