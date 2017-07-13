//
//  World.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__World__
#define __noctisgames__World__

#include <memory>
#include <vector>

class Entity;
class b2World;
class Robot;
class Ground;

class EntityContactListener;

class World
{
public:
    static Entity* sClientCreateRobot();
    static Entity* sServerCreateRobot();
    
    static Entity* sClientCreateProjectile();
    static Entity* sServerCreateProjectile();
    
    static Entity* sClientCreateSpacePirate();
    static Entity* sServerCreateSpacePirate();
    
    World(bool isServer);
    
    ~World();
    
    void addEntity(Entity* inEntity);
    
    void removeEntity(Entity* inEntity);
    
    void update();
    
    Robot* getRobotWithPlayerId(uint8_t inPlayerID);
    
    void killAllSpacePirates();
    
    bool hasSpacePirates();
    
    std::vector<Entity*>& getEntities();
    
private:
    std::vector<Entity*> m_entities;
    b2World* m_world;
    Ground* m_ground;
    EntityContactListener* m_entityContactListener;
    bool m_isServer;
    
    b2World& getWorld();
};

#include "Box2D/Dynamics/b2WorldCallbacks.h"

class b2Contact;

class EntityContactListener : public b2ContactListener
{
    virtual void BeginContact(b2Contact* contact);
};

#endif /* defined(__noctisgames__World__) */
