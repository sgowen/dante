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
class b2World;

class EntityContactListener;
class EntityContactFilter;

class World
{
public:
    World(bool isServer);
    ~World();
    
    void addEntity(Entity* inEntity);
    void removeEntity(Entity* inEntity);
    void postRead();
    void update();
    Entity* getRobotWithPlayerId(uint8_t inPlayerID);
    std::vector<Entity*>& getPlayers();
    std::vector<Entity*>& getEntities();
    b2World& getWorld();
    
private:
    std::vector<Entity*> _players;
    std::vector<Entity*> _entities;
    b2World* _world;
    EntityContactListener* _entityContactListener;
    EntityContactFilter* _entityContactFilter;
    bool _isServer;
    
    void stepPhysics();
    
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
