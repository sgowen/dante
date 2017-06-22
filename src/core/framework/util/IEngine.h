//
//  IEngine.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__IEngine__
#define __noctisgames__IEngine__

#include "RTTI.h"

class IEngine
{
    RTTI_DECL;
    
public:
    IEngine();
    
    virtual ~IEngine();
    
    virtual void createDeviceDependentResources() = 0;
    
    virtual void createWindowSizeDependentResources(int renderWidth, int renderHeight, int touchScreenWidth, int touchScreenHeight) = 0;
    
    virtual void releaseDeviceDependentResources() = 0;
    
    virtual void onResume() = 0;
	
    virtual void onPause() = 0;
    
    virtual void update(float deltaTime) = 0;
    
    virtual void render() = 0;
    
    int getRequestedAction();
    
    void clearRequestedAction();
    
protected:
    int m_iRequestedAction;
};

#endif /* defined(__noctisgames__IEngine__) */
