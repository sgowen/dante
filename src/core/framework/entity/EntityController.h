//
//  EntityController.h
//  noctisgames
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityController__
#define __noctisgames__EntityController__

#define DECL_EntityController_create \
public: \
    static EntityController* create(Entity* e)

#define IMPL_EntityController_create(name) \
EntityController* name::create(Entity* e)  \
{                                          \
    return new name(e);                    \
}                                          \

#include <framework/util/RTTI.h>

#include <vector>
#include <string>

class Entity;
class b2Fixture;

class EntityController
{
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    EntityController(Entity* inEntity);
    virtual ~EntityController();
    
    virtual void update();
    virtual void postUpdate();
    virtual void receiveMessage(uint16_t message, void* data = NULL);
    virtual void onFixturesCreated(std::vector<b2Fixture*>& fixtures);
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
protected:
    Entity* _entity;
};

#endif /* defined(__noctisgames__EntityController__) */
