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

#include <unordered_map>

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
    
    IInputState* getInputState() const;
    
    float getTimestamp() const;
    
private:
    class EntityClientCache
    {
    public:
        float m_fStateTime;
        b2Vec2 m_velocity;
        b2Vec2 m_position;
        float m_fAngle;
    };
    
    mutable std::unordered_map<int, EntityClientCache> m_entityCacheMap;
    
    IInputState* m_inputState;
    float m_fTimestamp;
};

#endif /* defined(__noctisgames__Move__) */
