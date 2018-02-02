//
//  Engine.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/main/portable/Engine.h"

#include "framework/main/portable/EngineController.h"
#include "framework/main/portable/EngineState.h"

#include "framework/util/FrameworkConstants.h"
#include "framework/util/FPSUtil.h"

NGRTTI_IMPL_NOPARENT(Engine);

Engine::Engine(EngineController* engineController) :
_engineController(engineController),
_stateMachine(this),
_frameStateTime(0),
_requestedAction(REQUESTED_ACTION_UPDATE),
_screenWidth(0),
_screenHeight(0),
_cursorWidth(0),
_cursorHeight(0)
{
    _stateMachine.setCurrentState(engineController->getInitialState());
}

Engine::~Engine()
{
    // Empty
}

void Engine::createDeviceDependentResources()
{
    _stateMachine.getCurrentState()->createDeviceDependentResources();
}

void Engine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    _cursorWidth = cursorWidth > 0 ? cursorWidth : screenWidth;
    _cursorHeight = cursorHeight > 0 ? cursorHeight : screenHeight;
    
    _stateMachine.getCurrentState()->createWindowSizeDependentResources(_screenWidth, _screenHeight, _cursorWidth, _cursorHeight);
}

void Engine::releaseDeviceDependentResources()
{
    _stateMachine.getCurrentState()->releaseDeviceDependentResources();
}

void Engine::onResume()
{
    _stateMachine.getCurrentState()->onResume();
}

void Engine::onPause()
{
    _stateMachine.getCurrentState()->onPause();
}

void Engine::update(double deltaTime)
{
    FPSUtil::getInstance()->update(deltaTime);
    
    _frameStateTime += deltaTime;
    
    while (_frameStateTime >= FRAME_RATE)
    {
        _frameStateTime -= FRAME_RATE;
        
        _stateMachine.update();
    }
}

void Engine::render()
{
    _stateMachine.getCurrentState()->render(_frameStateTime / FRAME_RATE);
}

EngineController* Engine::getEngineController()
{
    return _engineController;
}

StateMachine<Engine, EngineState>& Engine::getStateMachine()
{
    return _stateMachine;
}

void Engine::setRequestedAction(int inValue)
{
    _requestedAction = inValue;
}

int Engine::getRequestedAction()
{
    return _requestedAction;
}

void Engine::clearRequestedAction()
{
    _requestedAction = REQUESTED_ACTION_UPDATE;
}

int Engine::getScreenWidth()
{
    return _screenWidth;
}

int Engine::getScreenHeight()
{
    return _screenHeight;
}

int Engine::getCursorWidth()
{
    return _cursorWidth;
}

int Engine::getCursorHeight()
{
    return _cursorHeight;
}
