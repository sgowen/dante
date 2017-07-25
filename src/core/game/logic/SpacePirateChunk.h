//
//  SpacePirateChunk.h
//  dante
//
//  Created by Stephen Gowen on 7/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SpacePirateChunk__
#define __noctisgames__SpacePirateChunk__

#include "Entity.h"

#include "GameConstants.h"
#include "Box2D/Common/b2Math.h"

#include "RTTI.h"

class Ground;

class SpacePirateChunk : public Entity
{
    RTTI_DECL;
    
    NW_TYPE_DECL(NW_TYPE_SpacePirateChunk);
    
public:
    enum SpacePirateChunkReplicationState
    {
        CRAT_Pose = 1 << 0,
        
        CRAT_AllState = CRAT_Pose
    };
    
    SpacePirateChunk(b2World& world, bool isServer);
    
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

#endif /* defined(__noctisgames__SpacePirateChunk__) */
