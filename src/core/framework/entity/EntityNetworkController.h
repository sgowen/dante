//
//  EntityNetworkController.h
//  noctisgames
//
//  Created by Stephen Gowen on 4/17/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityNetworkController__
#define __noctisgames__EntityNetworkController__

#define DECL_EntityNetworkController_create \
public: \
    static EntityNetworkController* create(Entity* e, bool isServer)

#define IMPL_EntityNetworkController_create(name)                \
EntityNetworkController* name::create(Entity* e, bool isServer)  \
{                                                                \
    return new name(e, isServer);                                \
}                                                                \

#include <framework/util/RTTI.h>

class Entity;
class InputMemoryBitStream;
class OutputMemoryBitStream;

class EntityNetworkController
{
    DECL_RTTI;
    DECL_EntityNetworkController_create;
    
public:
    EntityNetworkController(Entity* e, bool isServer);
    virtual ~EntityNetworkController();
    
    virtual void read(InputMemoryBitStream& ip);
    virtual uint16_t write(OutputMemoryBitStream& op, uint16_t dirtyState);
    
    virtual void recallNetworkCache();
    virtual uint16_t getDirtyState();
    
    bool isServer();
    
protected:
    Entity* _entity;
    bool _isServer;
};

#endif /* defined(__noctisgames__EntityNetworkController__) */
