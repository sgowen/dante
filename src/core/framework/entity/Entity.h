//
//  Entity.h
//  noctisgames
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Entity__
#define __noctisgames__Entity__

#include <framework/util/Constants.h>
#include <framework/util/MathUtil.h>
#include <Box2D/Common/b2Math.h>

#include <framework/util/RTTI.h>

#include <stdint.h>
#include <map>
#include <vector>

class EntityController;
class EntityNetworkController;
class b2World;
class b2Body;
class b2Fixture;
class b2Contact;

enum FixtureFlags
{
    FixtureFlag_Box = 1 << 0,
    FixtureFlag_Sensor = 1 << 1,
    FixtureFlag_GroundContact = 1 << 2,
    FixtureFlag_Circle = 1 << 3
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
    BodyFlag_Bullet = 1 << 2,
    BodyFlag_Water = 1 << 3
};

struct EntityDef
{
    std::string name;
    std::string typeName;
    uint32_t ID;
    uint32_t type;
    std::string controller;
    std::map<int, std::string> textureMappings;
    std::map<int, int> soundMappings;
    std::vector<FixtureDef> fixtures;
    int bodyFlags;
    int x;
    int y;
    int width;
    int height;
    bool stateSensitive;
    bool server;
};

class Entity
{
    friend class EntityController;
    friend class EntityNetworkController;
    
    DECL_RTTI;
    
public:
    enum ReadStateFlag
    {
        ReadStateFlag_Pose =  1 << 0,
        ReadStateFlag_State = 1 << 1
    };
    
    Entity(EntityDef inEntityDef);
    ~Entity();
    
    void update();
    void interpolate(double alpha);
    void endInterpolation();
    bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    void initPhysics(b2World& world);
    void deinitPhysics();
    void updatePoseFromBody();
    void updateBodyFromPose();
    
    EntityDef& getEntityDef();
    EntityController* getController();
    EntityNetworkController* getNetworkController();
    uint16_t getStateTime();
    b2Body* getBody();
    void setPosition(b2Vec2 position);
    const b2Vec2& getPosition();
    void setVelocity(b2Vec2 velocity);
    const b2Vec2& getVelocity();
    float getWidth();
    float getHeight();
    void setAngle(float angle);
    float getAngle();
    const uint32_t getID();
    bool isGrounded();
    void requestDeletion();
    bool isRequestingDeletion();
    bool isServer();
    bool isFacingLeft();
    std::string& getTextureMapping();
    std::string& getTextureMapping(uint8_t state);
    int getSoundMapping(int state);
    bool isFixedRotation() const;
    
    struct Pose
    {
        b2Vec2 velocity;
        b2Vec2 position;
        float angle;
        uint8_t numGroundContacts;
        bool isFacingLeft;
        
        Pose(float x, float y)
        {
            velocity = b2Vec2_zero;
            position = b2Vec2(x, y);
            angle = 0;
            numGroundContacts = 0;
            isFacingLeft = false;
        }
        
#define NG_CLOSE_ENOUGH_IS_FINE
        
        friend bool operator==(Pose& lhs, Pose& rhs)
        {
            return
#ifdef NG_CLOSE_ENOUGH_IS_FINE
            isCloseEnough(lhs.velocity,            rhs.velocity) &&
            isCloseEnough(lhs.position,            rhs.position) &&
            isCloseEnough(lhs.angle,               rhs.angle) &&
#else
                          lhs.velocity          == rhs.velocity &&
                          lhs.position          == rhs.position &&
                          lhs.angle             == rhs.angle &&
#endif
                          lhs.numGroundContacts == rhs.numGroundContacts &&
                          lhs.isFacingLeft      == rhs.isFacingLeft;
        }
        
        friend bool operator!=(Pose& lhs, Pose& rhs)
        {
            return !(lhs == rhs);
        }
    };
    Pose& getPose();
    Pose& getPoseNetworkCache();
    
    struct State
    {
        uint16_t stateTime;
        uint8_t state;
        uint8_t stateFlags;
        
        State()
        {
            stateTime = 0;
            state = 0;
            stateFlags = 0;
        }
        
        friend bool operator==(State& lhs, State& rhs)
        {
            return
            lhs.stateTime         == rhs.stateTime &&
            lhs.state             == rhs.state &&
            lhs.stateFlags        == rhs.stateFlags;
        }
        
        friend bool operator!=(State& lhs, State& rhs)
        {
            return !(lhs == rhs);
        }
    };
    State& getState();
    State& getStateNetworkCache();
    
private:
    EntityDef _entityDef;
    EntityController* _controller;
    EntityNetworkController* _networkController;
    
    b2Body* _body;
    std::vector<b2Fixture*> _fixtures;
    b2Fixture* _groundSensorFixture;
    
    Pose _pose;
    Pose _poseNetworkCache;
    Pose _poseInterpolateCache;
    
    State _state;
    State _stateNetworkCache;
    
    const uint32_t _ID;
    float _deadZoneY;
    bool _isRequestingDeletion;
    bool _isBodyFacingLeft;
    
    void createFixtures();
    void destroyFixtures();
};

#endif /* defined(__noctisgames__Entity__) */
