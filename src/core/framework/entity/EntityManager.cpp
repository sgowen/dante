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

EntityManager::EntityManager() : m_isInitialized(false)
{
    // Empty
}

void EntityManager::init(HandleEntityDeletionFunc handleEntityDeletionFunc)
{
    m_handleEntityDeletion = handleEntityDeletionFunc;
    m_isInitialized = true;
}

Entity* EntityManager::getEntityFromID(int id)const
{
    auto q = m_entityMap.find(id);
    
    if (q != m_entityMap.end())
    {
        return q->second;
    }
    
    return nullptr;
}

void EntityManager::registerEntity(Entity* inEntity)
{
    m_entityMap.insert(std::make_pair(inEntity->getID(), inEntity));
}

void EntityManager::removeEntity(Entity* inEntity)
{
    if (m_isInitialized)
    {
        m_handleEntityDeletion(inEntity);
    }
    
    m_entityMap.erase(inEntity->getID());
    
    delete inEntity;
    inEntity = nullptr;
}

void EntityManager::reset()
{
    for (std::unordered_map<int, Entity*>::iterator i = m_entityMap.begin(); i != m_entityMap.end(); )
    {
        if (m_isInitialized)
        {
            m_handleEntityDeletion(i->second);
        }
        
        delete i->second;
        
        i = m_entityMap.erase(i);
    }
}

std::unordered_map<int, Entity*>& EntityManager::getMap()
{
    return m_entityMap;
}

std::unordered_map<int, Entity*> EntityManager::getMapCopy()
{
    return m_entityMap;
}
