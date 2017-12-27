//
//  Engine.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Engine__
#define __noctisgames__Engine__

#include "framework/util/NGRTTI.h"

class Engine
{
    NGRTTI_DECL;
    
public:
    Engine();
    virtual ~Engine();
    
    virtual void createDeviceDependentResources() = 0;
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight) = 0;
    virtual void releaseDeviceDependentResources() = 0;
    
    virtual void onResume() = 0;
    virtual void onPause() = 0;
    
    virtual void update(double deltaTime) = 0;
    virtual void render() = 0;
    
    int getRequestedAction();
    void clearRequestedAction();
    
protected:
    double _stateTime;
    double _frameStateTime;
    
    int _requestedAction;
    int _engineState;
};

#endif /* defined(__noctisgames__Engine__) */
