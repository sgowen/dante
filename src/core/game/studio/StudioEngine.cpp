//
//  StudioEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/studio/StudioEngine.h>

#include <framework/file/portable/JsonFile.h>
#include <game/studio/StudioRenderer.h>

#include <game/logic/Server.h>
#include <framework/util/Constants.h>
#include <framework/input/CursorInputManager.h>
#include <framework/input/CursorEvent.h>
#include <framework/input/CursorConverter.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <framework/network/portable/SocketAddressFactory.h>
#include <framework/network/portable/SocketUtil.h>
#include <game/studio/StudioInputManager.h>
#include <game/logic/World.h>
#include <game/game/GameInputState.h>
#include <framework/entity/EntityManager.h>
#include <framework/network/client/SocketClientHelper.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/NGExtension.h>
#include <framework/util/PlatformHelper.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/Constants.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/Timing.h>
#include <framework/util/FPSUtil.h>
#include <framework/input/CursorConverter.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <game/logic/GameConfig.h>
#include <framework/util/macros.h>
#include <framework/network/client/SocketClientHelper.h>
#include <game/game/GameEngine.h>
#include <game/game/GameInputManager.h>
#include <game/title/TitleEngine.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamClientHelper.h>
#include <framework/network/steam/NGSteamAddress.h>
#include <framework/network/steam/NGSteamGameServices.h>
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
_world(new World(WorldFlag_MapLoadAll)),
_state(StudioEngineState_DisplayGrid | StudioEngineState_DisplayControls | StudioEngineState_LayerAll)
{
    StudioInputManager::create();
    StudioInputManager::getInstance()->setEngine(this);
    
    _renderer->setEngine(this);
    _renderer->setInputManager(StudioInputManager::getInstance());
}

StudioEngine::~StudioEngine()
{
    delete _renderer;
    delete _world;
}

void StudioEngine::enter(Engine* engine)
{
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    SET_BIT(_state, StudioEngineState_TestSession, false);
    
    engine->getStateMachine().setPreviousState(TitleEngine::getInstance());
}

void StudioEngine::update(Engine* engine)
{
    if (_state & StudioEngineState_TestSession)
    {
        if (NG_SERVER && NG_SERVER->isConnected())
        {
            NetworkManagerClient::create(new SocketClientHelper(std::string("localhost:9999"), std::string("TEST"), GM_CFG->_clientPort, NG_CLIENT_CALLBACKS), GAME_ENGINE_CALLBACKS, INPUT_MANAGER_CALLBACKS);
            
            assert(NG_CLIENT);
            
            engine->getStateMachine().changeState(GameEngine::getInstance());
        }
    }
    else
    {
        StudioInputManager::getInstance()->update();
        
        if (StudioInputManager::getInstance()->getMenuState() == SIMS_ESCAPE)
        {
            engine->getStateMachine().revertToPreviousState();
        }
    }
}

void StudioEngine::exit(Engine* engine)
{
    releaseDeviceDependentResources();
}

void StudioEngine::createDeviceDependentResources()
{
    GM_CFG->initWithJsonFile("global.cfg");
    EntityMapper::getInstance()->initWithJsonFile("entities.cfg");
    EntityLayoutMapper::getInstance()->initWithJsonFile("maps.cfg");
    ASSETS->initWithJsonFile("game_assets.cfg");
    
    CURSOR_CONVERTER->setCamSize(GM_CFG->_camWidth, GM_CFG->_camHeight);
    
    _renderer->createDeviceDependentResources();
}

void StudioEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight, GM_CFG->_framebufferWidth, GM_CFG->_framebufferHeight);
    
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

void StudioEngine::render(double alpha)
{
    _renderer->render();
}
