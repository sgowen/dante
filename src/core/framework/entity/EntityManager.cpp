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

EntityManager::EntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc) : m_handleEntityCreatedFunc(handleEntityCreatedFunc), m_handleEntityDeletionFunc(handleEntityDeletionFunc)
{
    // Empty
}

EntityManager::~EntityManager()
{
    NGSTDUtil::cleanUpMapOfEntityPointers(m_entityMap);
}

Entity* EntityManager::getEntityByID(uint32_t inID) const
{
    auto q = m_entityMap.find(inID);
    
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

void EntityManager::deregisterEntity(Entity* inEntity)
{
    m_entityMap.erase(inEntity->getID());
    
    m_handleEntityDeletionFunc(inEntity);
    
    inEntity->onDeletion();
    
    delete inEntity;
    inEntity = nullptr;
}

std::unordered_map<uint32_t, Entity*>& EntityManager::getMap()
{
    return m_entityMap;
}

std::unordered_map<uint32_t, Entity*> EntityManager::getMapCopy()
{
    return m_entityMap;
}
