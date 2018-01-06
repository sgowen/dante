//
//  EntityController.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityController__
#define __noctisgames__EntityController__

#include "framework/util/NGRTTI.h"

class Entity;
class InputMemoryBitStream;
class OutputMemoryBitStream;
class b2Fixture;

class EntityController
{
    NGRTTI_DECL;
    
public:
    EntityController(Entity* inEntity);
    virtual ~EntityController();
    
    virtual void update() = 0;
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    virtual uint32_t getAllStateMask() const = 0;
    virtual void read(InputMemoryBitStream& inInputStream) = 0;
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) = 0;
    virtual bool needsMoveReplay() = 0;
    
protected:
    Entity* _entity;
};

#endif /* defined(__noctisgames__EntityController__) */
