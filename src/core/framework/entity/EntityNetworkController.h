//
//  EntityNetworkController.h
//  noctisgames
//
//  Created by Stephen Gowen on 4/17/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityNetworkController__
#define __noctisgames__EntityNetworkController__

#define DECL_EntityNetworkController_create \
public: \
    static EntityNetworkController* create(Entity* e)

#define IMPL_EntityNetworkController_create(name) \
EntityNetworkController* name::create(Entity* e)  \
{                                                 \
    return new name(e);                           \
}                                                 \

#include <framework/util/RTTI.h>

#include <vector>
#include <string>

class Entity;
class InputMemoryBitStream;
class OutputMemoryBitStream;

class EntityNetworkController
{
    DECL_RTTI;
    DECL_EntityNetworkController_create;
    
public:
    EntityNetworkController(Entity* inEntity);
    virtual ~EntityNetworkController();
    
    virtual void read(InputMemoryBitStream& ip);
    virtual void recallCache();
    virtual uint16_t write(OutputMemoryBitStream& op, uint16_t dirtyState);
    
protected:
    Entity* _entity;
};

#endif /* defined(__noctisgames__EntityNetworkController__) */
