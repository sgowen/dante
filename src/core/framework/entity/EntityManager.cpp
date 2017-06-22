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
#include "NGSTDUtil.h"

#include <cassert>

EntityManager::EntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc) : m_handleEntityCreatedFunc(handleEntityCreatedFunc), m_handleEntityDeletionFunc(handleEntityDeletionFunc)
{
    // Empty
}

EntityManager::~EntityManager()
{
    NGSTDUtil::cleanUpMapOfPointers(m_entityMap);
}

Entity* EntityManager::getEntityByID(int id)const
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
    
    m_handleEntityCreatedFunc(inEntity);
}

void EntityManager::removeEntity(Entity* inEntity)
{
    m_entityMap.erase(inEntity->getID());
    
    m_handleEntityDeletionFunc(inEntity);
    
    delete inEntity;
    inEntity = nullptr;
}

std::unordered_map<int, Entity*>& EntityManager::getMap()
{
    return m_entityMap;
}

std::unordered_map<int, Entity*> EntityManager::getMapCopy()
{
    return m_entityMap;
}
