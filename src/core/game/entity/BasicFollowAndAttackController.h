//
//  BasicFollowAndAttackController.h
//  dante
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__BasicFollowAndAttackController__
#define __noctisgames__BasicFollowAndAttackController__

#include <framework/entity/EntityController.h>

class BasicFollowAndAttackController : public EntityController
{
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    BasicFollowAndAttackController(Entity* inEntity);
    virtual ~BasicFollowAndAttackController();
    
    virtual void update();
    virtual void postUpdate();
    virtual void receiveMessage(uint16_t message, void* data = NULL);
    virtual void onFixturesCreated(std::vector<b2Fixture*>& fixtures);
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
private:
    enum State
    {
        State_Idle = 0,
        State_Moving,
        State_Attacking,
        State_Dying
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
    Stats _statsCache;
    
    /// Non-Networked
    b2Fixture* _attackSensorFixture;
    
    /// Server Only
    uint32_t _target;
    uint32_t _targetTouching;
    
    void handleIdleState();
    void handleMovingState();
    void handleAttackingState();
    void handleDyingState();
};

#endif /* defined(__noctisgames__BasicFollowAndAttackController__) */
