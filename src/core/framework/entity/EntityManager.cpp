//
//  EntityManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/28/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "EntityManager.h"

#include "Entity.h"

#include "StringUtil.h"

#include <cassert>

EntityManager* EntityManager::getInstance()
{
    static EntityManager instance = EntityManager();

    return &instance;
}

Entity* EntityManager::getEntityFromID(int id)const
{
    auto q = m_entityMap.find(id);
    
    if (q != m_entityMap.end())
    {
        return q->second;
    }
    
    LOG("<EntityManager::getEntityFromID>: invalid ID");
    
    return nullptr;
}

void EntityManager::registerEntity(Entity* entity)
{
    m_entityMap.insert(std::make_pair(entity->getID(), entity));
}

void EntityManager::removeEntity(Entity* entity)
{
    m_entityMap.erase(entity->getID());
}

void EntityManager::reset()
{
    m_entityMap.clear();
}

std::unordered_map<int, Entity*>& EntityManager::getMap()
{
    return m_entityMap;
}

std::unordered_map<int, Entity*> EntityManager::getMapCopy()
{
    return m_entityMap;
}

EntityManager::EntityManager()
{
    // Hide Constructor for Singleton
}
