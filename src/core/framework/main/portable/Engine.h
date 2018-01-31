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
#include "framework/util/StateMachine.h"

class EngineController;
class EngineState;

class Engine
{
    NGRTTI_DECL;
    
public:
    Engine(EngineController* engineController);
    ~Engine();
    
    void createDeviceDependentResources();
    void createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth = 0, int cursorHeight = 0);
    void releaseDeviceDependentResources();
    void onResume();
    void onPause();
    void update(double deltaTime);
    void render();
    EngineController* getEngineController();
    StateMachine<Engine, EngineState>& getStateMachine();
    void setRequestedAction(int inValue);
    int getRequestedAction();
    void clearRequestedAction();
    int getScreenWidth();
    int getScreenHeight();
    int getRenderWidth();
    int getRenderHeight();
    int getCursorWidth();
    int getCursorHeight();
    bool areDeviceDependentResourcesCreated();
    
private:
    EngineController* _engineController;
    StateMachine<Engine, EngineState> _stateMachine;
    double _frameStateTime;
    int _requestedAction;
    int _screenWidth;
    int _screenHeight;
    int _cursorWidth;
    int _cursorHeight;
};

#endif /* defined(__noctisgames__Engine__) */
