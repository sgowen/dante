//
//  GameObject.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameObject__
#define __noctisgames__GameObject__

#include "NetworkConstants.h"
#include "Color.h"
#include "Vector2.h"
#include "macros.h"

#include "RTTI.h"

class RoboCat;
class OutputMemoryBitStream;
class InputMemoryBitStream;

class GameObject
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_GameObject);
    
public:
    GameObject();
    
    virtual ~GameObject() {}
    
    virtual uint32_t getAllStateMask() const { return 0; }
    
    //return whether to keep processing collision
    virtual bool handleCollisionWithCat(RoboCat* inCat)
    {
        UNUSED(inCat);
        
        return true;
    }
    
    virtual void update();
    
    virtual void handleDying() {}
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
    {
        UNUSED(inOutputStream);
        UNUSED(inDirtyState);
        
        return 0;
    }
    
    virtual void read(InputMemoryBitStream& inInputStream)
    {
        UNUSED(inInputStream);
    }
    
    void SetIndexInWorld(int inIndex) { m_iIndexInWorld = inIndex; }
    int GetIndexInWorld() const { return m_iIndexInWorld; }
    
    void SetRotation(float inRotation);
    float GetRotation() const { return m_fRotation; }
    
    Vector2& GetLocation() { return m_location; }
    void SetLocation(Vector2 inLocation) { m_location = inLocation; }
    
    float GetCollisionRadius() const { return m_fCollisionRadius; }
    void SetCollisionRadius(float inRadius) { m_fCollisionRadius = inRadius; }
    
    Vector2 GetForwardVector() const;
    
    void SetColor(Color& inColor) { m_color = inColor; }
    Color& GetColor() { return m_color; }
    
    bool DoesWantToDie() const { return m_doesWantToDie; }
    void SetDoesWantToDie(bool inWants) { m_doesWantToDie = inWants; }
    
    int GetNetworkId() const { return m_iNetworkId; }
    void SetNetworkId(int inNetworkId);
    
private:
    Vector2 m_location;
    Color m_color;
    
    float m_fCollisionRadius;
    
    float m_fRotation;
    int m_iIndexInWorld;
    
    bool m_doesWantToDie;
    
    int m_iNetworkId;
};

#endif /* defined(__noctisgames__GameObject__) */
