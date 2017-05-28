//
//  GameObject.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "GameObject.h"

#include "RoboCat.h"
#include "MemoryBitStream.h"
#include "Vector2.h"

#include <math.h>

GameObject::GameObject() :
m_iIndexInWorld(-1),
m_fCollisionRadius(0.5f),
mDoesWantToDie(false),
m_fRotation(0.f),
m_iNetworkId(0),
mColor(1.0f, 1.0f, 1.0f, 1.0f),
m_fScale(1.0f)
{
    // Empty
}

void GameObject::Update()
{
    //object don't do anything by default...
}

Vector2 GameObject::GetForwardVector()	const
{
    //should we cache this when you turn?
    return Vector2(sinf(m_fRotation), -cosf(m_fRotation));
}

void GameObject::SetNetworkId(int inNetworkId)
{
    //this doesn't put you in the map or remove you from it
    m_iNetworkId = inNetworkId;
}

void GameObject::SetRotation(float inRotation)
{
    //should we normalize using fmodf?
    m_fRotation = inRotation;
}
