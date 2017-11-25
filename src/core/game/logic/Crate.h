//
//  Crate.h
//  dante
//
//  Created by Stephen Gowen on 7/23/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Crate__
#define __noctisgames__Crate__

#include "framework/entity/Entity.h"

#include "game/logic/GameConstants.h"
#include "Box2D/Common/b2Math.h"

#include "framework/util/NGRTTI.h"

class Ground;

class Crate : public Entity
{
    NGRTTI_DECL;
    
    NW_TYPE_DECL(NW_TYPE_Crate);
    
public:
    enum CrateReplicationState
    {
        CRAT_Pose = 1 << 0,
        
        CRAT_AllState = CRAT_Pose
    };
    
    Crate(b2World& world, bool isServer);
    
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

#endif /* defined(__noctisgames__Crate__) */
