//
//  EngineState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/27/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EngineState__
#define __noctisgames__EngineState__

#include "framework/util/NGRTTI.h"
#include <framework/util/State.h>
#include <framework/main/portable/Engine.h>

class EngineState : public State<Engine>
{
    NGRTTI_DECL;
    
public:
    virtual void enter(Engine* engine) = 0;
    virtual void update(Engine* engine) = 0;
    virtual void exit(Engine* engine) = 0;
    virtual void createDeviceDependentResources() = 0;
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight) = 0;
    virtual void releaseDeviceDependentResources() = 0;
    virtual void onResume() = 0;
    virtual void onPause() = 0;
    virtual void render(double alpha) = 0;

protected:
    EngineState();
    virtual ~EngineState();
};

#endif /* defined(__noctisgames__EngineState__) */
