//
//  EntityManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/28/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityManager__
#define __noctisgames__EntityManager__

#include <unordered_map>

class Entity;

typedef void (*HandleEntityCreatedFunc)(Entity* inEntity);
typedef void (*HandleEntityDeletionFunc)(Entity* inEntity);

class EntityManager
{
public:
    EntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc);
    
    ~EntityManager();
    
    Entity* getEntityByID(uint32_t inID) const;
    
    void registerEntity(Entity* inEntity);
    
    void deregisterEntity(Entity* inEntity);
    
    std::unordered_map<uint32_t, Entity*>& getMap();
    
    std::unordered_map<uint32_t, Entity*> getMapCopy();
    
private:
    HandleEntityCreatedFunc m_handleEntityCreatedFunc;
    HandleEntityDeletionFunc m_handleEntityDeletionFunc;
    
    std::unordered_map<uint32_t, Entity*> m_entityMap;
};

#endif /* defined(__noctisgames__EntityManager__) */
