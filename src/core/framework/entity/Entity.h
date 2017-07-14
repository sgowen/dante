//
//  Entity.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Entity__
#define __noctisgames__Entity__

#include "Color.h"
#include "FrameworkConstants.h"

#include "RTTI.h"

#include <stdint.h>

class b2World;
struct b2Vec2;
class b2Body;
class b2Fixture;
class OutputMemoryBitStream;
class InputMemoryBitStream;

#define NETWORK_TYPE_DECL(inCode) \
public: \
enum { kClassId = inCode }; \
virtual uint32_t getNetworkType();

#define NETWORK_TYPE_IMPL(name) \
uint32_t name::getNetworkType() { return kClassId; }

class Entity
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_Entity);
    
public:
    Entity(b2World& world, float x, float y, float width, float height, bool isStaticBody = false);
    
    virtual ~Entity();
    
    virtual void update() = 0;
    
    virtual void handleContact(Entity* inEntity) = 0;
    
    virtual uint32_t getAllStateMask() const = 0;
    
    virtual void read(InputMemoryBitStream& inInputStream) = 0;
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) = 0;
    
    void setStateTime(float stateTime);
    
    float getStateTime();
    
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
    
    void setID(int inID);
    
    int getID();
    
    void requestDeletion();
    
    bool isRequestingDeletion();
    
protected:
    b2World& m_worldRef;
    b2Body* m_body;
    b2Fixture* m_fixture;
    float m_fStateTime;
    Color m_color;
    float m_fWidth;
    float m_fHeight;
    
private:
    int m_iID;
    bool m_isRequestingDeletion;
    
    static int getUniqueEntityID();
};

#endif /* defined(__noctisgames__Entity__) */
