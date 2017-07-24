//
//  Crate.h
//  dante
//
//  Created by Stephen Gowen on 7/23/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Crate__
#define __noctisgames__Crate__

#include "Entity.h"

#include "GameConstants.h"
#include "Box2D/Common/b2Math.h"

#include "RTTI.h"

class Ground;

class Crate : public Entity
{
    RTTI_DECL;
    
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
    
    virtual bool shouldCollide(Entity* inEntity);
    
    virtual void handleContact(Entity* inEntity);
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void handleContactWithGround(Ground* inGround);
    
private:
    bool m_isServer;
    
    // Cached Values
    b2Vec2 m_velocityOld;
    b2Vec2 m_positionOld;
};

#endif /* defined(__noctisgames__Crate__) */
