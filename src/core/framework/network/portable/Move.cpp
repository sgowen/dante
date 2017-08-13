//
//  Move.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Move.h"

#include "IInputState.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Entity.h"

Move::Move(IInputState* inInputState, float inTimestamp) :
m_inputState(inInputState),
m_fTimestamp(inTimestamp)
{
    // Empty
}

Move::Move(IInputState* inInputState) :
m_inputState(inInputState),
m_fTimestamp(0)
{
    // Empty
}

Move::~Move()
{
    m_entityCacheMap.clear();
    
    m_inputState->setInUse(false);
}

bool Move::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write(m_fTimestamp);
    
    return m_inputState->write(inOutputStream);
}

bool Move::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(m_fTimestamp);
    
    return m_inputState->read(inInputStream);
}

void Move::cacheEntity(Entity* inEntity) const
{
    EntityClientCache entityClientCache;
    
    entityClientCache.m_fStateTime = inEntity->getStateTime();
    entityClientCache.m_velocity = b2Vec2(inEntity->getVelocity().x, inEntity->getVelocity().y);
    entityClientCache.m_position = b2Vec2(inEntity->getPosition().x, inEntity->getPosition().y);
    entityClientCache.m_fAngle = inEntity->getAngle();
    
    m_entityCacheMap.insert(std::make_pair(inEntity->getID(), entityClientCache));
}

void Move::recallEntityCache(Entity* inEntity) const
{
    auto q = m_entityCacheMap.find(inEntity->getID());
    
    if (q != m_entityCacheMap.end())
    {
        inEntity->setStateTime(q->second.m_fStateTime);
        inEntity->setVelocity(q->second.m_velocity);
        inEntity->setPosition(q->second.m_position);
        inEntity->setAngle(q->second.m_fAngle);
    }
}

bool Move::isEqual(const Move* inMove) const
{
    return m_inputState->isEqual(inMove->getInputState());
}

IInputState* Move::getInputState() const
{
    return m_inputState;
}

float Move::getTimestamp() const
{
    return m_fTimestamp;
}

void Move::setTimestamp(float timeStamp)
{
    m_fTimestamp = timeStamp;
}

void Move::copyInputState(IInputState* inIInputState)
{
    inIInputState->copyTo(m_inputState);
}
