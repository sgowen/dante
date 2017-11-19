//
//  World.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__World__
#define __noctisgames__World__

#include "framework/util/FrameworkConstants.h"

#include <memory>
#include <vector>

class Entity;
class Robot;
class Projectile;
class SpacePirate;
class Crate;
class SpacePirateChunk;
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
    WORLD_CREATE_CLIENT_DECL(SpacePirateChunk);
    
    WORLD_CREATE_SERVER_DECL(Robot);
    WORLD_CREATE_SERVER_DECL(Projectile);
    WORLD_CREATE_SERVER_DECL(SpacePirate);
    WORLD_CREATE_SERVER_DECL(Crate);
    WORLD_CREATE_SERVER_DECL(SpacePirateChunk);
    
    World(bool isServer);
    
    ~World();
    
    void addEntity(Entity* inEntity);
    
    void removeEntity(Entity* inEntity);
    
    void postRead();
    
    void update();
    
    Robot* getRobotWithPlayerId(uint8_t inPlayerID);
    
    void killAllSpacePirates();
    
    void removeAllCrates();
    
    bool hasSpacePirates();
    
    bool hasCrates();
    
    std::vector<Entity*>& getPlayers();
    
    std::vector<Entity*>& getEntities();
    
    b2World& getWorld();
    
private:
    std::vector<Entity*> _players;
    std::vector<Entity*> _entities;
    b2World* _world;
    Ground* _ground;
    EntityContactListener* _entityContactListener;
    EntityContactFilter* _entityContactFilter;
    bool _isServer;
    
    void stepPhysics(float deltaTime);
    
    int getMoveCount();
};

#include "Box2D/Dynamics/b2WorldCallbacks.h"

class b2Contact;

class EntityContactListener : public b2ContactListener
{
    virtual void BeginContact(b2Contact* contact);
    
    virtual void EndContact(b2Contact* contact);
};

class b2Fixture;

class EntityContactFilter : public b2ContactFilter
{
    virtual bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
};

#endif /* defined(__noctisgames__World__) */
