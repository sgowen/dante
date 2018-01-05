//
//  MainEngineController.h
//  dante
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MainEngineController__
#define __noctisgames__MainEngineController__

#include "framework/main/portable/EngineController.h"

class MainEngineController : public EngineController
{
    NGRTTI_DECL;
    
public:
    MainEngineController();
    virtual ~MainEngineController();
    
    virtual EngineState* getInitialState();
};

#endif /* defined(__noctisgames__MainEngineController__) */
