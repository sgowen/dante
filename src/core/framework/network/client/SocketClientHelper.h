//
//  SocketClientHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketClientHelper__
#define __noctisgames__SocketClientHelper__

#include "IClientHelper.h"

class SocketClientHelper : public IClientHelper
{
public:
    SocketClientHelper();
    
    virtual ~SocketClientHelper();
};

#endif /* defined(__noctisgames__SocketClientHelper__) */
