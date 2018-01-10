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
#include "framework/math/MathUtil.h"
#include "Box2D/Common/b2Math.h"

#include "framework/util/NGRTTI.h"

#include <stdint.h>
#include <map>
#include <vector>

class b2World;
class b2Body;
class b2Fixture;
class b2Contact;
class OutputMemoryBitStream;
class InputMemoryBitStream;
class Move;
class EntityController;

enum FixtureFlags
{
    FixtureFlag_Box = 1 << 0,
    FixtureFlag_Sensor = 1 << 1,
    FixtureFlag_GroundContact = 1 << 2
};

struct FixtureDef
{
    std::vector<b2Vec2> vertices;
    b2Vec2 center;
    float restitution;
    float density;
    float friction;
    int flags;
};

enum BodyFlags
{
    BodyFlag_Static = 1 << 0,
    BodyFlag_FixedRotation = 1 << 1,
    BodyFlag_Bullet = 1 << 2
};

struct EntityDef
{
    uint32_t type;
    std::string controller;
    std::map<std::string, std::string> mappings;
    std::vector<FixtureDef> fixtures;
    int bodyFlags;
    float width;
    float height;
    int layer;
    bool stateSensitive;
};

class Entity
{
    NGRTTI_DECL;
    
public:
    Entity(EntityDef& inEntityDef, bool isServer);
    ~Entity();
    
    void update();
    bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void read(InputMemoryBitStream& inInputStream);
    void recallLastReadState();
    uint16_t write(OutputMemoryBitStream& inOutputStream, uint16_t inDirtyState);
    
    void initPhysics(b2World& world);
    void deinitPhysics(b2World& world);
    EntityDef& getEntityDef();
    EntityController* getEntityController();
    void setStateTime(float stateTime);
    float getStateTime();
    b2Body* getBody();
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
    bool isServer();
    bool isFacingLeft();
    std::string& getTextureMapping();
    
    struct Pose
    {
        float stateTime;
        uint8_t state;
        b2Vec2 velocity;
        b2Vec2 position;
        float angle;
        uint8_t numGroundContacts;
        bool isFacingLeft;
        
        Pose()
        {
            stateTime = 0;
            state = 0;
            velocity = b2Vec2_zero;
            position = b2Vec2_zero;
            angle = 0;
            numGroundContacts = 0;
            isFacingLeft = false;
        }
        
        friend bool operator==(Pose& lhs, Pose& rhs)
        {
            return
            lhs.stateTime         == rhs.stateTime &&
            lhs.state             == rhs.state &&
            lhs.velocity          == rhs.velocity &&
            lhs.position          == rhs.position &&
            lhs.angle             == rhs.angle &&
            lhs.numGroundContacts == rhs.numGroundContacts &&
            lhs.isFacingLeft      == rhs.isFacingLeft;
        }
        
        friend bool operator!=(Pose& lhs, Pose& rhs)
        {
            return !(lhs == rhs);
        }
    };
    Pose& getPose();
    
private:
    enum ReadStateFlag
    {
        ReadStateFlag_Pose = 1 << 0
    };
    
    EntityDef& _entityDef;
    EntityController* _controller;
    bool _isServer;
    
    /// Physics
    b2Body* _body;
    std::vector<b2Fixture*> _fixtures;
    b2Fixture* _groundSensorFixture;
    
    /// Network
    Pose _pose;
    Pose _poseCache;
    
    uint16_t _readState;
    uint8_t _state;
    uint32_t _ID;
    bool _isRequestingDeletion;
    
    void updatePoseFromBody();
    void updateBodyFromPose();
};

#endif /* defined(__noctisgames__Entity__) */
