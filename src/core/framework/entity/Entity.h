//
//  Entity.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Entity__
#define __noctisgames__Entity__

#include "framework/graphics/portable/Color.h"
#include "framework/util/FrameworkConstants.h"

#include "Box2D/Common/b2Math.h"

#include "framework/util/NGRTTI.h"

#include <stdint.h>

class b2World;
struct b2Vec2;
class b2Body;
class b2Fixture;
class b2Contact;
class OutputMemoryBitStream;
class InputMemoryBitStream;
class Move;
class EntityController;

struct EntityDef
{
    uint32_t type;
    std::string controller;
    float width;
    float height;
    float restitution;
    float density;
    float friction;
    bool staticBody;
    bool fixedRotation;
    bool bullet;
    bool sensor;
    bool character;
    bool stateSensitive;
    
    EntityDef()
    {
        type = 'ENTY';
        controller = "DefaultController";
        width = 1;
        height = 1;
        restitution = 0;
        density = 0;
        friction = 0.2f;
        staticBody = true;
        fixedRotation = true;
        bullet = false;
        sensor = false;
        character = false;
        stateSensitive = false;
    }
};

class Entity
{
    /// TODO, fixme
    friend class RobotController;
    
    NGRTTI_DECL;
    
public:
    Entity(EntityDef& inEntityDef, b2World& world, bool isServer);
    ~Entity();
    
    void update();
    bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void read(InputMemoryBitStream& inInputStream);
    uint16_t write(OutputMemoryBitStream& inOutputStream, uint16_t inDirtyState);
    void recallLastReadState();
    
    void deinitPhysics();
    void handleBeginFootContact(Entity* inEntity);
    void handleEndFootContact(Entity* inEntity);
    EntityDef& getEntityDef();
    EntityController* getEntityController();
    void setStateTime(float stateTime);
    float getStateTime();
    b2Body* getBody();
    void setTransform(b2Vec2 position, float angle);
    void setPosition(b2Vec2 position);
    const b2Vec2& getPosition();
    void setVelocity(b2Vec2 velocity);
    const b2Vec2& getVelocity();
    float getWidth();
    float getHeight();
    void setAngle(float angle);
    float getAngle();
    void setID(uint32_t inID);
    uint32_t getID();
    bool isGrounded();
    bool isFalling();
    void requestDeletion();
    bool isRequestingDeletion();
    
private:
    enum ReplicationState
    {
        ENTY_Pose = 1 << 0
    };
    
    EntityDef& _entityDef;
    EntityController* _controller;
    b2World& _worldRef;
    bool _isServer;
    
    /// Physics
    b2Body* _body;
    b2Fixture* _fixture;
    b2Fixture* _groundSensorFixture;
    
    struct Pose
    {
        float stateTime;
        uint8_t state;
        b2Vec2 velocity;
        b2Vec2 position;
        float angle;
        uint8_t numGroundContacts;
        
        Pose()
        {
            stateTime = 0;
            state = 0;
            velocity = b2Vec2_zero;
            position = b2Vec2_zero;
            angle = 0;
            numGroundContacts = 0;
        }
        
        friend bool operator==(Pose& lhs, Pose& rhs)
        {
            return
            // Don't force a network write if only stateTime is different
            lhs.stateTime         == rhs.stateTime &&
            lhs.state             == rhs.state &&
            lhs.velocity          == rhs.velocity &&
            lhs.position          == rhs.position &&
            lhs.angle             == rhs.angle &&
            lhs.numGroundContacts == rhs.numGroundContacts;
        }
        
        friend bool operator!=(Pose& lhs, Pose& rhs)
        {
            return !(lhs == rhs);
        }
    };
    Pose _pose;
    Pose _poseCache;
    
    uint16_t _readState;
    
    uint32_t _ID;
    bool _isRequestingDeletion;
};

#endif /* defined(__noctisgames__Entity__) */
