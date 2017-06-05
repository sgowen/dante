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

World* World::getInstance()
{
    static World instance = World();
    return &instance;
}

void World::staticAddEntity(Entity* inEntity)
{
    World::getInstance()->addEntity(inEntity);
}

void World::staticRemoveEntity(Entity* inEntity)
{
    World::getInstance()->removeEntity(inEntity);
}

Robot* World::staticGetRobotWithPlayerId(int playerId)
{
    const std::vector<Entity*>& entities = World::getInstance()->getEntities();
    
    int len = static_cast<int>(entities.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* ent = entities[i];
        if (ent->getRTTI().derivesFrom(Robot::rtti))
        {
            Robot* robot = static_cast<Robot*>(ent);
            if (robot->getPlayerId() == playerId)
            {
                return robot;
            }
        }
    }
    
    return nullptr;
}

void World::addEntity(Entity* inEntity)
{
    m_objects.push_back(inEntity);
}

void World::removeEntity(Entity* inEntity)
{
    int len = static_cast<int>(m_objects.size());
    int indexToRemove = -1;
    for (int i = 0; i < len; ++i)
    {
        if (m_objects[i]->getID() == inEntity->getID())
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
            m_objects[indexToRemove] = m_objects[lastIndex];
        }
        
        m_objects.pop_back();
    }
}

void World::update()
{
    //update all game objects- sometimes they want to die, so we need to tread carefully...
    
    int len = static_cast<int>(m_objects.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* go = m_objects[i];
        
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

const std::vector<Entity*>& World::getEntities() const
{
    return m_objects;
}

World::World()
{
    // Empty
}
