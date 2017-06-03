//
//  Entity.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Entity__
#define __noctisgames__Entity__

#include "Vector2.h"
#include "Color.h"
#include "FrameworkConstants.h"

#include "RTTI.h"

#include <stdint.h>
#include <vector>

class NGRect;
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
    Entity(float x, float y, float width, float height);
    
    virtual ~Entity();
    
    virtual void onDeletion();
    
    virtual void update();
    
    virtual void update(float inDeltaTime);
    
    virtual void resetBounds(float width, float height);
    
    virtual void updateBounds();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    float getStateTime();
    
    void setStateTime(float stateTime);
    
    void setPosition(Vector2 position);
    
    Vector2& getPosition();
    
    Vector2& getVelocity();
    
    Vector2& getAcceleration();
    
    std::vector<NGRect *>& getBounds();
    
    NGRect& getMainBounds();
    
    void setColor(Color color);
    
    Color& getColor();
    
    const float& getWidth();
    
    void setWidth(float width);
    
    const float& getHeight();
    
    void setHeight(float height);
    
    void setAngle(float angle);
    
    float getAngle();
    
    int getID();
    
    void setID(int inID);
    
    void requestDeletion();
    
    bool isRequestingDeletion();
    
protected:
    float m_fStateTime;
    Vector2 m_position;
    Vector2 m_velocity;
    Vector2 m_acceleration;
    std::vector<NGRect *> m_bounds;
    Color m_color;
    float m_fWidth;
    float m_fHeight;
    float m_fAngle;
    
private:
    int m_iID;
    bool m_isRequestingDeletion;
    
    static int getUniqueEntityID();
};

#endif /* defined(__noctisgames__Entity__) */
