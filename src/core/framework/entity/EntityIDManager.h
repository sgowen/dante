//
//  EntityIDManager.h
//  noctisgames
//
//  Created by Stephen Gowen on 3/28/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityIDManager__
#define __noctisgames__EntityIDManager__

#include <stdint.h>

class EntityIDManager
{
public:
    EntityIDManager();
    ~EntityIDManager();
    
    void reset();
    
    uint32_t getNextStaticEntityID();
    uint32_t getNextDynamicEntityID();
    
private:
    uint32_t _staticEntityID;
    uint32_t _dynamicEntityID;
};

#endif /* defined(__noctisgames__EntityIDManager__) */
