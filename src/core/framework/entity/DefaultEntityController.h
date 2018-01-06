//
//  DefaultEntityController.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DefaultEntityController__
#define __noctisgames__DefaultEntityController__

#include <framework/entity/EntityController.h>

class DefaultEntityController : public EntityController
{
    NGRTTI_DECL;
    
public:
    static EntityController* create(Entity* inEntity);
    
    DefaultEntityController(Entity* inEntity);
    virtual ~DefaultEntityController();
    
    virtual void update();
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual uint32_t getAllStateMask() const;
    virtual void read(InputMemoryBitStream& inInputStream);
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    virtual bool needsMoveReplay();
};

#endif /* defined(__noctisgames__DefaultEntityController__) */
