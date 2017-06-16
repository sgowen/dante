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

typedef void (*HandleEntityDeletionFunc)(Entity* inEntity);

class EntityManager
{
public:
    EntityManager();
    
    void init(HandleEntityDeletionFunc handleEntityDeletionFunc);
    
    Entity* getEntityFromID(int id)const;
    
    void registerEntity(Entity* inEntity);
    
    void removeEntity(Entity* inEntity);
    
    void reset();
    
    std::unordered_map<int, Entity*>& getMap();
    
    std::unordered_map<int, Entity*> getMapCopy();
    
private:
    std::unordered_map<int, Entity*> m_entityMap;
    HandleEntityDeletionFunc m_handleEntityDeletion;
    bool m_isInitialized;
};

#endif /* defined(__noctisgames__EntityManager__) */
