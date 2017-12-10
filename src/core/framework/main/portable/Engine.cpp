//
//  Engine.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/main/portable/Engine.h"

#include "framework/graphics/portable/Renderer.h"

#include "framework/util/FrameworkConstants.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "framework/util/Timing.h"
#include "framework/util/FPSUtil.h"
#include "framework/input/CursorConverter.h"

NGRTTI_IMPL_NOPARENT(Engine);

Engine::Engine(Renderer* inRenderer) :
_renderer(inRenderer),
_stateTime(0),
_frameStateTime(0),
_requestedAction(REQUESTED_ACTION_UPDATE),
_engineState(0)
{
    // Empty
}

Engine::~Engine()
{
    delete _renderer;
}

void Engine::createDeviceDependentResources()
{
    _renderer->createDeviceDependentResources();
}

void Engine::createWindowSizeDependentResources(int renderWidth, int renderHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(renderWidth, renderHeight, NUM_FRAMEBUFFERS);
    
    CURSOR_CONVERTER->setCursorSize(cursorWidth, cursorHeight);
}

void Engine::releaseDeviceDependentResources()
{
    _renderer->releaseDeviceDependentResources();
    
    NG_AUDIO_ENGINE->reset();
}

void Engine::onResume()
{
    NG_AUDIO_ENGINE->resume();
}

void Engine::onPause()
{
    NG_AUDIO_ENGINE->pause();
}

void Engine::update(float deltaTime)
{
    FPSUtil::getInstance()->update(deltaTime);
    
    _frameStateTime += deltaTime;
    
    if (_frameStateTime >= FRAME_RATE)
    {
        while (_frameStateTime >= FRAME_RATE)
        {
            _frameStateTime -= FRAME_RATE;
            _stateTime += FRAME_RATE;
            
            Timing::getInstance()->updateManual(_stateTime, FRAME_RATE);
            
            onFrame();
        }
    }
}

void Engine::render()
{
    _renderer->render(_engineState);
}

int Engine::getRequestedAction()
{
    return _requestedAction;
}

void Engine::clearRequestedAction()
{
    _requestedAction = REQUESTED_ACTION_UPDATE;
}
