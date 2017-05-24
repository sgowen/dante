//
//  GameObject.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameObject__
#define __noctisgames__GameObject__

#include "RoboMath.h"

#include <memory>

class RoboCat;
class OutputMemoryBitStream;
class InputMemoryBitStream;

#define CLASS_IDENTIFICATION(inCode, inClass) \
enum { kClassId = inCode }; \
virtual uint32_t GetClassId() const { return kClassId; } \
static GameObject* CreateInstance() { return static_cast<GameObject*>(new inClass()); } \

#define SUB_CLASS_IDENTIFICATION(inCode, inClass) \
enum { kClassId = inCode }; \
virtual uint32_t GetClassId() const override { return kClassId; } \
static GameObject* CreateInstance() { return static_cast<GameObject*>(new inClass()); } \

class GameObject
{
public:
    CLASS_IDENTIFICATION('GOBJ', GameObject)
    
    GameObject();
    virtual ~GameObject() {}
    
    virtual	RoboCat* GetAsCat()	{ return nullptr; }
    
    virtual uint32_t GetAllStateMask() const { return 0; }
    
    //return whether to keep processing collision
    virtual bool HandleCollisionWithCat(RoboCat* inCat) { (void) inCat; return true; }
    
    virtual void Update();
    
    virtual void HandleDying() {}
    
    void SetIndexInWorld(int inIndex) { m_iIndexInWorld = inIndex; }
    int GetIndexInWorld() const { return m_iIndexInWorld; }
    
    void SetRotation(float inRotation);
    float GetRotation() const { return m_fRotation; }
    
    void SetScale(float inScale) { m_fScale = inScale; }
    float GetScale() const { return m_fScale; }
    
    const Vector3& GetLocation() const { return mLocation; }
    void SetLocation(const Vector3& inLocation) { mLocation = inLocation; }
    
    float GetCollisionRadius() const { return m_fCollisionRadius; }
    void SetCollisionRadius(float inRadius) { m_fCollisionRadius = inRadius; }
    
    Vector3 GetForwardVector() const;
    
    void SetColor(const Vector3& inColor) { mColor = inColor; }
    const Vector3& GetColor() const { return mColor; }
    
    bool DoesWantToDie() const { return mDoesWantToDie; }
    void SetDoesWantToDie(bool inWants) { mDoesWantToDie = inWants; }
    
    int GetNetworkId() const { return m_iNetworkId; }
    void SetNetworkId(int inNetworkId);
    
    virtual uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const { (void)inOutputStream; (void)inDirtyState; return 0; }
    virtual void Read(InputMemoryBitStream& inInputStream) { (void)inInputStream; }
    
private:
    Vector3 mLocation;
    Vector3 mColor;
    
    float m_fCollisionRadius;
    
    float m_fRotation;
    float m_fScale;
    int m_iIndexInWorld;
    
    bool mDoesWantToDie;
    
    int m_iNetworkId;
};

typedef std::shared_ptr<GameObject> GameObjectPtr;

#endif /* defined(__noctisgames__GameObject__) */
