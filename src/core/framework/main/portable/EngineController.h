//
//  EngineController.h
//  noctisgames
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EngineController__
#define __noctisgames__EngineController__

#include <framework/util/RTTI.h>

class EngineState;

class EngineController
{
    DECL_RTTI;
    
public:
    EngineController();
    virtual ~EngineController();
    
    virtual EngineState* getInitialState() = 0;
};

#endif /* defined(__noctisgames__EngineController__) */
