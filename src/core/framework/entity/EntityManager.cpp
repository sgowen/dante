//
//  EntityManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/28/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/entity/EntityManager.h"

#include "framework/entity/Entity.h"

#include "framework/util/StringUtil.h"
#include "framework/util/NGSTDUtil.h"

EntityManager::EntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc) : _handleEntityCreatedFunc(handleEntityCreatedFunc), _handleEntityDeletionFunc(handleEntityDeletionFunc),
_entityID(0)
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
    
    return NULL;
}

void EntityManager::registerEntity(Entity* inEntity)
{
    ++_entityID;
    
    inEntity->setID(_entityID);
    
    _entityMap.insert(std::make_pair(inEntity->getID(), inEntity));
    
    _handleEntityCreatedFunc(inEntity);
}

void EntityManager::deregisterEntity(Entity* inEntity)
{
    _entityMap.erase(inEntity->getID());
    
    _handleEntityDeletionFunc(inEntity);
    
    inEntity->onDeletion();
    
    delete inEntity;
    inEntity = NULL;
}

void EntityManager::setEntityID(uint32_t inValue)
{
    _entityID = inValue;
}

std::map<uint32_t, Entity*>& EntityManager::getMap()
{
    return _entityMap;
}
