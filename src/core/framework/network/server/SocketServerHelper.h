//
//  SocketServerHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketServerHelper__
#define __noctisgames__SocketServerHelper__

#include "IServerHelper.h"

class SocketServerHelper : public IServerHelper
{
public:
    SocketServerHelper();
    
    virtual ~SocketServerHelper();
};

#endif /* defined(__noctisgames__SocketServerHelper__) */
