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
_inputState(inInputState),
_timestamp(inTimestamp)
{
    // Empty
}

Move::Move(IInputState* inInputState) :
_inputState(inInputState),
_timestamp(0)
{
    // Empty
}

Move::~Move()
{
    _entityCacheMap.clear();
}

bool Move::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write(_timestamp);
    
    return _inputState->write(inOutputStream);
}

bool Move::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(_timestamp);
    
    return _inputState->read(inInputStream);
}

void Move::cacheEntity(Entity* inEntity) const
{
    EntityClientCache entityClientCache;
    
    entityClientCache._stateTime = inEntity->getStateTime();
    entityClientCache._velocity = b2Vec2(inEntity->getVelocity().x, inEntity->getVelocity().y);
    entityClientCache._position = b2Vec2(inEntity->getPosition().x, inEntity->getPosition().y);
    entityClientCache._angle = inEntity->getAngle();
    
    _entityCacheMap.insert(std::make_pair(inEntity->getID(), entityClientCache));
}

void Move::recallEntityCache(Entity* inEntity) const
{
    auto q = _entityCacheMap.find(inEntity->getID());
    
    if (q != _entityCacheMap.end())
    {
        inEntity->setStateTime(q->second._stateTime);
        inEntity->setVelocity(q->second._velocity);
        inEntity->setPosition(q->second._position);
        inEntity->setAngle(q->second._angle);
    }
}

bool Move::isEqual(const Move* inMove) const
{
    return _inputState->isEqual(inMove->getInputState());
}

IInputState* Move::getInputState() const
{
    return _inputState;
}

float Move::getTimestamp() const
{
    return _timestamp;
}

void Move::setTimestamp(float timeStamp)
{
    _timestamp = timeStamp;
}

void Move::copyInputState(IInputState* inIInputState)
{
    inIInputState->copyTo(_inputState);
}
