//
//  GameObject.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "GameObject.h"

#include "MemoryBitStream.h"

GameObject::GameObject() :
mIndexInWorld(-1),
m_fCollisionRadius(0.5f),
mDoesWantToDie(false),
m_fRotation(0.f),
mNetworkId(0),
mColor(Colors::White),
m_fScale(1.0f)
{
}

void GameObject::Update()
{
    //object don't do anything by default...
}

Vector3 GameObject::GetForwardVector()	const
{
    //should we cache this when you turn?
    return Vector3(sinf(m_fRotation), -cosf(m_fRotation), 0.f);
}

void GameObject::SetNetworkId(int inNetworkId)
{
    //this doesn't put you in the map or remove you from it
    mNetworkId = inNetworkId;
}

void GameObject::SetRotation(float inRotation)
{
    //should we normalize using fmodf?
    m_fRotation = inRotation;
}
