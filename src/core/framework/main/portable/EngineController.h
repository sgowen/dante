//
//  EngineController.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EngineController__
#define __noctisgames__EngineController__

#include <framework/util/NGRTTI.h>

class EngineState;

class EngineController
{
    NGRTTI_DECL;
    
public:
    EngineController();
    virtual ~EngineController();
    
    virtual EngineState* getInitialState() = 0;
};

#endif /* defined(__noctisgames__EngineController__) */
