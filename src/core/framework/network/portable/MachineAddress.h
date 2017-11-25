//
//  MachineAddress.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MachineAddress__
#define __noctisgames__MachineAddress__

#include "framework/util/NGRTTI.h"

#include <string>

class MachineAddress
{
    NGRTTI_DECL;
    
public:
    MachineAddress();
    
    virtual ~MachineAddress();
    
    virtual MachineAddress* createNewCopy() = 0;
    
    virtual uint64_t getHash() const = 0;
    
    virtual std::string toString() const = 0;
};

#endif /* defined(__noctisgames__MachineAddress__) */
