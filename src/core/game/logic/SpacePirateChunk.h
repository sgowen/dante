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

class SpacePirate;

#define Space_Pirate_Chunk_Top_Left 0
#define Space_Pirate_Chunk_Top_Right 1
#define Space_Pirate_Chunk_Bottom_Left 2
#define Space_Pirate_Chunk_Bottom_Right 3

class SpacePirateChunk : public Entity
{
    RTTI_DECL;
    
    NW_TYPE_DECL(NW_TYPE_SpacePirateChunk);
    
public:
    enum SpacePirateChunkReplicationState
    {
        SPCH_Pose = 1 << 0,
        
        SPCH_AllState = SPCH_Pose
    };
    
    SpacePirateChunk(b2World& world, bool isServer);
    
    virtual EntityDef constructEntityDef();
    
    virtual void update();
    
    virtual bool shouldCollide(Entity* inEntity);
    
    virtual void handleContact(Entity* inEntity);
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void initFromSpacePirate(SpacePirate* spacePirate, b2Vec2 force, int type);
    
    int getType();
    
    bool isFacingLeft();
    
private:
    int m_iType;
    bool m_isServer;
    bool m_isFacingLeft;
    
    // Cached Values
    b2Vec2 m_velocityOld;
    b2Vec2 m_positionOld;
};

#endif /* defined(__noctisgames__SpacePirateChunk__) */
