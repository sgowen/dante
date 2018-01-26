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
#include <string>

enum WorldFlags
{
    WorldFlag_Server = 1 << 0,
    WorldFlag_MapLoadAll = 1 << 1
};

class Entity;
class b2World;
class EntityContactListener;
class EntityContactFilter;

class World
{
public:
    World(int flags = 0);
    ~World();
    
    void addDynamicEntity(Entity* inEntity);
    void removeDynamicEntity(Entity* inEntity);
    void updateServer();
    void postRead();
    void updateClient();
    void interpolate(double alpha);
    void postRender();
    void loadMap(uint32_t map);
    void mapAddEntity(Entity* e);
    void mapRemoveEntity(Entity* e);
    void saveMap();
    void saveMapAs(uint32_t map);
    std::string& getMapName();
    std::string& getMapFileName();
    Entity* getPlayerWithId(uint8_t inPlayerID);
    std::vector<Entity*>& getPlayers();
    std::vector<Entity*>& getDynamicEntities();
    std::vector<Entity*>& getStaticEntities();
    std::vector<Entity*>& getLayers();
    b2World& getWorld();
    
private:
    std::vector<Entity*> _players;
    std::vector<Entity*> _dynamicEntities;
    std::vector<Entity*> _staticEntities;
    std::vector<Entity*> _layers;
    b2World* _world;
    EntityContactListener* _entityContactListener;
    EntityContactFilter* _entityContactFilter;
    uint32_t _map;
    std::string _mapFileName;
    std::string _mapName;
    int _flags;
    
    void stepPhysics();
    void clearDynamicEntities(std::vector<Entity*>& entities);
    void updateAndRemoveEntitiesAsNeeded(std::vector<Entity*>& entities);
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
