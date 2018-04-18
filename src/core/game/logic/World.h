//
//  World.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__World__
#define __noctisgames__World__

#include <framework/util/Constants.h>

#include <memory>
#include <vector>
#include <string>

enum WorldFlags
{
    WorldFlag_Server =     1 << 0,
    WorldFlag_Studio = 1 << 1
};

class Entity;
class EntityIDManager;
struct EntityDef;
class MoveList;
class Move;
class b2World;

class EntityContactListener;
class EntityContactFilter;

class World
{
public:
    World(uint32_t flags = 0);
    ~World();
    
    void loadMap(uint32_t map);
    void saveMap();
    void saveMapAs(uint32_t map);
    void addEntity(Entity* e);
    void removeEntity(Entity* e);
    void updateServer();
    void postRead(MoveList& moveList);
    void updateClient(const Move* pendingMove);
    void interpolate(double alpha);
    void endInterpolation();
    void clear();
    bool isMapLoaded();
    std::string& getMapName();
    std::string& getMapFileName();
    std::vector<Entity*>& getPlayers();
    std::vector<Entity*>& getDynamicEntities();
    std::vector<Entity*>& getWaterBodies();
    std::vector<Entity*>& getStaticEntities();
    std::vector<Entity*>& getLayers();
    b2World& getWorld();
    
private:
    uint32_t _flags;
    b2World* _world;
    EntityContactListener* _entityContactListener;
    EntityContactFilter* _entityContactFilter;
    EntityIDManager* _entityIDManager;
    std::vector<Entity*> _players;
    std::vector<Entity*> _dynamicEntities;
    std::vector<Entity*> _waterBodies;
    std::vector<Entity*> _staticEntities;
    std::vector<Entity*> _layers;
    uint32_t _map;
    std::string _mapFileName;
    std::string _mapName;
    
    void stepPhysics();
    void clearDynamicEntities(std::vector<Entity*>& entities);
    void updateAndRemoveEntitiesAsNeeded(std::vector<Entity*>& entities);
    void handleDirtyStates(std::vector<Entity*>& entities);
    
    bool isLayer(Entity* e);
    bool isWater(Entity* e);
    bool isStatic(Entity* e);
    bool isDynamic(Entity* e);
    bool isDynamic(EntityDef& ed);
    
    void refreshPlayers();
    void removeEntity(Entity* e, std::vector<Entity*>& entities);
    void deinitPhysics(std::vector<Entity*>& entities);
};

#include <Box2D/Dynamics/b2WorldCallbacks.h>

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
