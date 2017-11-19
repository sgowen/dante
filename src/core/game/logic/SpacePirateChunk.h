//
//  SpacePirateChunk.h
//  dante
//
//  Created by Stephen Gowen on 7/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SpacePirateChunk__
#define __noctisgames__SpacePirateChunk__

#include "framework/entity/Entity.h"

#include "game/logic/GameConstants.h"
#include "Box2D/Common/b2Math.h"

#include "framework/util/RTTI.h"

class SpacePirate;

#define Space_Pirate_Chunk_Top_Left 0
#define Space_Pirate_Chunk_Top_Right 1
#define Space_Pirate_Chunk_Botto_Left 2
#define Space_Pirate_Chunk_Botto_Right 3

class SpacePirateChunk : public Entity
{
    RTTI_DECL;
    
    NW_TYPE_DECL(NW_TYPE_SpacePirateChunk);
    
public:
    enum SpacePirateChunkReplicationState
    {
        SPCH_Info = 1 << 0,
        SPCH_Pose = 1 << 1,
        
        SPCH_AllState = SPCH_Info | SPCH_Pose
    };
    
    SpacePirateChunk(b2World& world, bool isServer);
    
    virtual EntityDef constructEntityDef();
    
    virtual void update();
    
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    virtual bool needsMoveReplay();
    
    void initFromSpacePirate(SpacePirate* spacePirate, b2Vec2 force, int type);
    
    int getType();
    
    bool isFacingLeft();
    
private:
    int _type;
    bool _isFacingLeft;
};

#endif /* defined(__noctisgames__SpacePirateChunk__) */
