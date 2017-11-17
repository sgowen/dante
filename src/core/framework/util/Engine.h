//
//  Engine.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Engine__
#define __noctisgames__Engine__

#include "RTTI.h"

class Renderer;

class Engine
{
    RTTI_DECL;
    
public:
    Engine(Renderer* inRenderer);
    
    virtual ~Engine();
    
    virtual void createDeviceDependentResources();
    
    virtual void createWindowSizeDependentResources(int renderWidth, int renderHeight, int touchScreenWidth, int touchScreenHeight);
    
    virtual void releaseDeviceDependentResources();
    
    virtual void onResume();
	
    virtual void onPause();
    
    virtual void update(float deltaTime);
    
    virtual void render();
    
    int getRequestedAction();
    
    void clearRequestedAction();
    
protected:
    Renderer* _renderer;
    
    float _stateTime;
    float _frameStateTime;
    
    int _requestedAction;
    int _engineState;
    
    virtual void onFrame() = 0;
};

#endif /* defined(__noctisgames__Engine__) */
