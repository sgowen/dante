//
//  Move.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Move__
#define __noctisgames__Move__

#include "Box2D/Common/b2Math.h"

#include <map>

class IInputState;
class OutputMemoryBitStream;
class InputMemoryBitStream;
class Entity;

class Move
{
public:
    Move(IInputState* inInputState, float inTimestamp);
    
    Move(IInputState* inInputState);
    
    ~Move();
    
    bool write(OutputMemoryBitStream& inOutputStream) const;
    
    bool read(InputMemoryBitStream& inInputStream);
    
    void cacheEntity(Entity* inEntity) const;
    
    void recallEntityCache(Entity* inEntity) const;
    
    bool isEqual(const Move* inMove) const;
    
    IInputState* getInputState() const;
    
    float getTimestamp() const;
    
    void setTimestamp(float timeStamp);
    
    void copyInputState(IInputState* inIInputState);
    
private:
    class EntityClientCache
    {
    public:
        float _stateTime;
        b2Vec2 _velocity;
        b2Vec2 _position;
        float _angle;
    };
    
    mutable std::map<int, EntityClientCache> _entityCacheMap;
    
    IInputState* _inputState;
    float _timestamp;
};

#endif /* defined(__noctisgames__Move__) */
