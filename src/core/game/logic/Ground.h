//
//  Ground.h
//  dante
//
//  Created by Stephen Gowen on 7/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Ground__
#define __noctisgames__Ground__

#include "framework/entity/Entity.h"

#include "game/logic/GameConstants.h"

#include "framework/util/RTTI.h"

class Ground : public Entity
{
    RTTI_DECL;
    
    NW_TYPE_DECL(NW_TYPE_Ground);
    
public:
    Ground(b2World& world, bool isServer);
    
    virtual EntityDef constructEntityDef();
    
    virtual void update();
    
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    virtual bool needsMoveReplay();
};

#endif /* defined(__noctisgames__Ground__) */
