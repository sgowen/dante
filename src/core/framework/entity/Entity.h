//
//  Entity.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Entity__
#define __noctisgames__Entity__

#include "framework/math/Color.h"
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

#define NW_TYPE_DECL(inCode) \
public: \
enum { kClassId = inCode }; \
virtual uint32_t getNetworkType();

#define NW_TYPE_IMPL(name) \
uint32_t name::getNetworkType() { return kClassId; }

struct EntityDef
{
    EntityDef()
    {
        restitution = 0.0f;
        density = 1.0f;
        isStaticBody = true;
        fixedRotation = true;
        bullet = false;
        isSensor = false;
        isCharacter = false;
    }
    
    float restitution;
    float density;
    bool isStaticBody;
    bool fixedRotation;
    bool bullet;
    bool isSensor;
    bool isCharacter;
};

class Entity
{
    NGRTTI_DECL;
    
    NW_TYPE_DECL(NW_TYPE_Entity);
    
public:
    Entity(b2World& world, float x, float y, float width, float height, bool isServer, EntityDef inEntityDef, bool autoInitPhysics = true);
    
    virtual ~Entity();
    
    virtual EntityDef constructEntityDef() = 0;
    
    virtual void update() = 0;
    
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) = 0;
    
    virtual uint32_t getAllStateMask() const = 0;
    
    virtual void read(InputMemoryBitStream& inInputStream) = 0;
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) = 0;
    
    virtual bool needsMoveReplay() = 0;
    
    virtual void recallIfNecessary(Move* move);
    
    virtual void postRead();
    
    virtual void cacheToMove(const Move& move);
    
    virtual void onDeletion();
    
    void handleBeginFootContact(Entity* inEntity);
    
    void handleEndFootContact(Entity* inEntity);
    
    void setStateTime(float stateTime);
    
    float getStateTime();
    
    b2Body* getBody();
    
    void setTransform(b2Vec2 position, float angle);
    
    void setPosition(b2Vec2 position);
    
    const b2Vec2& getPosition();
    
    void setVelocity(b2Vec2 velocity);
    
    const b2Vec2& getVelocity();
    
    void setColor(Color color);
    
    Color& getColor();
    
    void setWidth(float width);
    
    const float& getWidth();
    
    void setHeight(float height);
    
    const float& getHeight();
    
    void setAngle(float angle);
    
    float getAngle();
    
    void setID(uint32_t inID);
    
    uint32_t getID();
    
    bool isGrounded();
    
    bool isFalling();
    
    void requestDeletion();
    
    bool isRequestingDeletion();
    
protected:
    b2World& _worldRef;
    b2Body* _body;
    b2Fixture* _fixture;
    b2Fixture* _footSensorFixture;
    float _stateTime;
    uint8_t _numGroundContacts;
    Color _color;
    float _x;
    float _y;
    float _width;
    float _height;
    float _angle;
    uint32_t _readState;
    bool _isServer;
    bool _isPhysicsOn;
    
    // Cached Last Known Values (from previous frame)
    b2Vec2 _velocityLastKnown;
    b2Vec2 _positionLastKnown;
    float _angleLastKnown;
    uint8_t _numGroundContactsLastKnown;
    
    void initPhysics(EntityDef inEntityDef);
    
    void deinitPhysics();
    
    void interpolateClientSidePrediction(b2Vec2& inOldVelocity, b2Vec2& inOldPos);
    
    bool interpolateVectorsIfNecessary(b2Vec2& inA, const b2Vec2& inB, float& syncTracker, const char* vectorType);
    
private:
    float _timeVelocityBecameOutOfSync;
    float _timePositionBecameOutOfSync;
    uint32_t _iD;
    bool _isRequestingDeletion;
    
    static uint32_t getUniqueEntityID();
};

#endif /* defined(__noctisgames__Entity__) */
