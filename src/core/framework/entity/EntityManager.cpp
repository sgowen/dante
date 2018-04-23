//
//  EntityManager.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 8/28/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/EntityManager.h>

#include <framework/entity/Entity.h>

EntityManager::EntityManager(HandleEntityCreatedFunc inHandleEntityCreatedFunc, HandleEntityDeletionFunc inHandleEntityDeletionFunc) :
_handleEntityCreatedFunc(inHandleEntityCreatedFunc),
_handleEntityDeletionFunc(inHandleEntityDeletionFunc)
{
    // Empty
}

EntityManager::~EntityManager()
{
    // Empty
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

void EntityManager::registerEntity(Entity* e)
{
    _entityMap.insert(std::make_pair(e->getID(), e));
    
    if (_handleEntityCreatedFunc)
    {
        _handleEntityCreatedFunc(e);
    }
}

void EntityManager::deregisterEntity(Entity* e)
{
    _entityMap.erase(e->getID());
    
    if (_handleEntityDeletionFunc)
    {
        _handleEntityDeletionFunc(e);
    }
}

std::map<uint32_t, Entity*>& EntityManager::getMap()
{
    return _entityMap;
}
