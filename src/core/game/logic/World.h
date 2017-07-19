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
class EntityContactFilter;

#define WORLD_CREATE_CLIENT_DECL(name) \
static Entity* sClientCreate##name()

#define WORLD_CREATE_SERVER_DECL(name) \
static Entity* sServerCreate##name()

class World
{
public:
    WORLD_CREATE_CLIENT_DECL(Robot);
    WORLD_CREATE_CLIENT_DECL(Projectile);
    WORLD_CREATE_CLIENT_DECL(SpacePirate);
    
    WORLD_CREATE_SERVER_DECL(Robot);
    WORLD_CREATE_SERVER_DECL(Projectile);
    WORLD_CREATE_SERVER_DECL(SpacePirate);
    
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
    EntityContactFilter* m_entityContactFilter;
    bool m_isServer;
    
    b2World& getWorld();
};

#include "Box2D/Dynamics/b2WorldCallbacks.h"

class b2Contact;

class EntityContactListener : public b2ContactListener
{
    virtual void BeginContact(b2Contact* contact);
};

class b2Fixture;

class EntityContactFilter : public b2ContactFilter
{
    virtual bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
};

#endif /* defined(__noctisgames__World__) */
