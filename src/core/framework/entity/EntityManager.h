//
//  EntityManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/28/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityManager__
#define __noctisgames__EntityManager__

#include <map>

class Entity;

class EntityManager
{
public:
    static EntityManager* getInstance();
    
    Entity* getEntityFromID(int id)const;
    
    void registerEntity(Entity* entity);
    
    void removeEntity(Entity* entity);
    
    void reset();
    
private:
    std::map<int, Entity*> m_entityMap;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityManager();
    EntityManager(const EntityManager&);
    EntityManager& operator=(const EntityManager&);
};

#endif /* defined(__noctisgames__EntityManager__) */
