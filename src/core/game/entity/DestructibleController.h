//
//  DestructibleController.h
//  dante
//
//  Created by Stephen Gowen on 2/2/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DestructibleController__
#define __noctisgames__DestructibleController__

#include <framework/entity/EntityController.h>

class DestructibleController : public EntityController
{
    NGRTTI_DECL;
    
public:
    static EntityController* create(Entity* inEntity);
    
    DestructibleController(Entity* inEntity);
    virtual ~DestructibleController();
    
    virtual void update(bool isLive = false);
    virtual void postUpdate();
    virtual void receiveMessage(uint16_t message, bool isLive, void* data = NULL);
    virtual void onFixturesCreated(std::vector<b2Fixture*>& fixtures);
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void read(InputMemoryBitStream& inInputStream, uint16_t& inReadState);
    virtual void recallLastReadState(uint16_t& inReadState);
    virtual uint16_t write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState);
    
private:
    enum State
    {
        State_Undamaged = 0,
        State_OneThirdDamaged,
        State_TwoThirdsDamaged,
        State_Destructing
    };
    
    enum ReadStateFlag
    {
        ReadStateFlag_Stats = 1 << 1
    };
    
    struct Stats
    {
        uint8_t health;
        
        Stats()
        {
            health = 3;
        }
        
        friend bool operator==(Stats& lhs, Stats& rhs)
        {
            return
            lhs.health == rhs.health;
        }
        
        friend bool operator!=(Stats& lhs, Stats& rhs)
        {
            return !(lhs == rhs);
        }
    };
    Stats _stats;
    Stats _statsCache;
};

#endif /* defined(__noctisgames__DestructibleController__) */
