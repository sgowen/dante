//
//  Entity.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Entity__
#define __noctisgames__Entity__

#include "FrameworkConstants.h"

#include "RTTI.h"

#include <stdint.h>

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
    Entity();
    
    virtual ~Entity();
    
    virtual void update();
    
    virtual void update(float deltaTime);
    
    virtual void onDeletion();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    int getID();
    
    void setID(int inID);
    
    float getStateTime();
    
    void setStateTime(float stateTime);
    
    void requestDeletion();
    
    bool isRequestingDeletion();
    
protected:
    float m_fStateTime;
    int m_ID;
    bool m_isRequestingDeletion;
    
private:
    static int getUniqueEntityID();
};

#endif /* defined(__noctisgames__Entity__) */
