//
//  IMachineAddress.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__IMachineAddress__
#define __noctisgames__IMachineAddress__

#include "RTTI.h"

#include <string>

class IMachineAddress
{
    RTTI_DECL;
    
public:
    IMachineAddress();
    
    virtual ~IMachineAddress();
    
    virtual IMachineAddress* createNewCopy() = 0;
    
    virtual uint64_t getHash() const = 0;
    
    virtual std::string toString() const = 0;
};

#endif /* defined(__noctisgames__IMachineAddress__) */
