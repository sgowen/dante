//
//  StudioEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/StudioEngine.h"

#include "framework/file/portable/JsonFile.h"
#include <game/graphics/portable/StudioRenderer.h>

#include "game/logic/Server.h"
#include "game/logic/GameConstants.h"
#include "framework/input/CursorInputManager.h"
#include "framework/input/CursorEvent.h"
#include "framework/input/CursorConverter.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/portable/SocketAddressFactory.h"
#include "framework/network/portable/SocketUtil.h"
#include "game/logic/StudioInputManager.h"
#include "game/logic/World.h"
#include "game/logic/InstanceManager.h"
#include "game/logic/MainInputState.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "framework/entity/EntityManager.h"
#include "framework/network/client/SocketClientHelper.h"
#include "framework/network/portable/MachineAddress.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "framework/util/NGExtension.h"
#include "framework/util/PlatformHelper.h"
#include "framework/util/FlagUtil.h"
#include "framework/graphics/portable/Assets.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "framework/util/Timing.h"
#include "framework/util/FPSUtil.h"
#include "framework/input/CursorConverter.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamClientHelper.h"
#include "framework/network/steam/NGSteamAddress.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

NGRTTI_IMPL(StudioEngine, EngineState);

StudioEngine* StudioEngine::s_instance = NULL;

void StudioEngine::create()
{
    assert(!s_instance);
    
    s_instance = new StudioEngine();
}

StudioEngine * StudioEngine::getInstance()
{
    return s_instance;
}

void StudioEngine::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

StudioEngine::StudioEngine() : EngineState(),
_renderer(new StudioRenderer()),
_state(SE_DEFAULT)
{
    // Empty
}

StudioEngine::~StudioEngine()
{
    delete _renderer;
}

void StudioEngine::enter(Engine* engine)
{
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getRenderWidth(), engine->getRenderHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    _state = SE_DEFAULT;
}

void StudioEngine::update(Engine* engine)
{
    StudioInputManager::getInstance()->update();
    
    if (handleInput())
    {
        engine->getStateMachine().revertToPreviousState();
        return;
    }
}

void StudioEngine::exit(Engine* engine)
{
    releaseDeviceDependentResources();
}

void StudioEngine::createDeviceDependentResources()
{
    _renderer->createDeviceDependentResources();
}

void StudioEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight, renderWidth, renderHeight);
    
    CURSOR_CONVERTER->setCamSize(CAM_WIDTH, CAM_HEIGHT);
    CURSOR_CONVERTER->setCursorSize(cursorWidth, cursorHeight);
}

void StudioEngine::releaseDeviceDependentResources()
{
    _renderer->releaseDeviceDependentResources();
}

void StudioEngine::onResume()
{
    // Empty
}

void StudioEngine::onPause()
{
    // Empty
}

void StudioEngine::render()
{
    _renderer->render(_state);
}

bool StudioEngine::handleInput()
{
    int menuState = StudioInputManager::getInstance()->getMenuState();
    
    if (menuState == MIS_ESCAPE)
    {
        return true;
    }
    
    return false;
}
