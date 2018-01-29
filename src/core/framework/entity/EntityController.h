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

#include <vector>
#include <string>

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
    
    virtual uint8_t update() = 0;
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    virtual void read(InputMemoryBitStream& inInputStream, uint16_t& inReadState) = 0;
    virtual void recallLastReadState(uint16_t& inReadState) = 0;
    virtual uint16_t write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState) = 0;
    
protected:
    Entity* _entity;
};

#endif /* defined(__noctisgames__EntityController__) */
