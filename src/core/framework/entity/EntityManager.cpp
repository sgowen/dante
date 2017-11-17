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

EntityManager::EntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc) : _handleEntityCreatedFunc(handleEntityCreatedFunc), _handleEntityDeletionFunc(handleEntityDeletionFunc)
{
    // Empty
}

EntityManager::~EntityManager()
{
    NGSTDUtil::cleanUpMapOfEntityPointers(_entityMap);
}

Entity* EntityManager::getEntityByID(uint32_t inID) const
{
    auto q = _entityMap.find(inID);
    
    if (q != _entityMap.end())
    {
        return q->second;
    }
    
    return nullptr;
}

void EntityManager::registerEntity(Entity* inEntity)
{
    _entityMap.insert(std::make_pair(inEntity->getID(), inEntity));
    
    _handleEntityCreatedFunc(inEntity);
}

void EntityManager::deregisterEntity(Entity* inEntity)
{
    _entityMap.erase(inEntity->getID());
    
    _handleEntityDeletionFunc(inEntity);
    
    inEntity->onDeletion();
    
    delete inEntity;
    inEntity = nullptr;
}

std::map<uint32_t, Entity*>& EntityManager::getMap()
{
    return _entityMap;
}

std::map<uint32_t, Entity*> EntityManager::getMapCopy()
{
    return _entityMap;
}
