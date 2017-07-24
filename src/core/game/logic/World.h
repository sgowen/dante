//
//  World.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__World__
#define __noctisgames__World__

#include "FrameworkConstants.h"

#include <memory>
#include <vector>

class Entity;
class Robot;
class Projectile;
class SpacePirate;
class Crate;
class Ground;
class b2World;

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
    WORLD_CREATE_CLIENT_DECL(Crate);
    
    WORLD_CREATE_SERVER_DECL(Robot);
    WORLD_CREATE_SERVER_DECL(Projectile);
    WORLD_CREATE_SERVER_DECL(SpacePirate);
    WORLD_CREATE_SERVER_DECL(Crate);
    
    World(bool isServer);
    
    ~World();
    
    void addEntity(Entity* inEntity);
    
    void removeEntity(Entity* inEntity);
    
    void postRead();
    
    void update();
    
    Robot* getRobotWithPlayerId(uint8_t inPlayerID);
    
    void killAllSpacePirates();
    
    bool hasSpacePirates();
    
    bool hasCrates();
    
    std::vector<Entity*>& getPlayers();
    
    std::vector<Entity*>& getEntities();
    
    b2World& getWorld(int index);
    
private:
    std::vector<Entity*> m_players;
    std::vector<Entity*> m_entities;
    b2World* m_world[MAX_NUM_PLAYERS_PER_SERVER]; // If server, 1 instance per connected client; otherwise, only use the first index
    Ground* m_ground[MAX_NUM_PLAYERS_PER_SERVER];
    EntityContactListener* m_entityContactListener;
    EntityContactFilter* m_entityContactFilter;
    bool m_isServer;
    
    void stepPhysics(float deltaTime);
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
