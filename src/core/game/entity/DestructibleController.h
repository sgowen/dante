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
    friend class DestructibleNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    DestructibleController(Entity* entity);
    virtual ~DestructibleController();
    
    virtual void update();
    virtual void receiveMessage(uint16_t message, void* data = NULL);
    virtual void onFixturesCreated(std::vector<b2Fixture*>& fixtures);
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
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
        ReadStateFlag_Stats = 1 << 2
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
    Stats _statsNetworkCache;
};

#include <framework/entity/EntityNetworkController.h>

class DestructibleNetworkController : public EntityNetworkController
{
    DECL_RTTI;
    DECL_EntityNetworkController_create;
    
public:
    DestructibleNetworkController(Entity* e, bool isServer);
    virtual ~DestructibleNetworkController();
    
    virtual void read(InputMemoryBitStream& ip);
    virtual uint16_t write(OutputMemoryBitStream& op, uint16_t dirtyState);
    
    virtual void recallNetworkCache();
    virtual uint16_t getDirtyState();
    
private:
    DestructibleController* _controller;
};

#endif /* defined(__noctisgames__DestructibleController__) */
