//
//  EntityRegistry.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityRegistry__
#define __noctisgames__EntityRegistry__

#include "NWPhysicalEntity.h"

#include "NetworkConstants.h"

#include <unordered_map>

typedef NWPhysicalEntity* (*NWPhysicalEntityCreationFunc)();

class EntityRegistry
{
public:
    static void StaticInit();
    
    static std::unique_ptr<EntityRegistry> sInstance;
    
    void RegisterCreationFunction(uint32_t inFourCCName, NWPhysicalEntityCreationFunc inCreationFunction);
    
    NWPhysicalEntity* CreateNWPhysicalEntity(uint32_t inFourCCName);
    
private:
    EntityRegistry();
    
    std::unordered_map<uint32_t, NWPhysicalEntityCreationFunc> mNameToNWPhysicalEntityCreationFunctionMap;
};

#endif /* defined(__noctisgames__EntityRegistry__) */
