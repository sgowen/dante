//
//  GameEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/game/GameEngine.h>

#include <game/game/GameRenderer.h>
#include <game/logic/World.h>
#include <game/game/GameInputManager.h>
#include <framework/util/Timing.h>
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
#include <game/logic/InstanceManager.h>
#include <game/game/GameInputState.h>
#include <framework/network/portable/FWInstanceManager.h>
#include <framework/entity/EntityManager.h>
#include <framework/network/client/SocketClientHelper.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/NGExtension.h>
#include <framework/util/PlatformHelper.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/Constants.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/FPSUtil.h>
#include <framework/input/CursorConverter.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <game/logic/GameConfig.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamClientHelper.h>
#include <framework/network/steam/NGSteamAddress.h>
#include <framework/network/steam/NGSteamGameServices.h>
#endif

NGRTTI_IMPL(GameEngine, EngineState);

GameEngine* GameEngine::s_instance = NULL;

void GameEngine::create()
{
    assert(!s_instance);
    
    s_instance = new GameEngine();
}

GameEngine * GameEngine::getInstance()
{
    return s_instance;
}

void GameEngine::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

GameEngine::GameEngine() : EngineState(),
_renderer(new GameRenderer()),
_world(NULL),
_input(NULL),
_timing(NULL),
_server(NULL),
_stateTime(0),
_state(GameEngineState_Default),
_map(0)
{
    _state |= GameEngineState_Interpolation | GameEngineState_Lighting | GameEngineState_DisplayUI;
    
    _renderer->setEngine(this);
}

GameEngine::~GameEngine()
{
    delete _renderer;
}

void GameEngine::enter(Engine* engine)
{
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    _map = 0;
    _stateTime = 0;
    
    GameInputManager::create();
    
    _world = new World();
    InstanceManager::setClientWorld(_world);
    _input = GameInputManager::getInstance();
    _timing = Timing::getInstance();
    _server = Server::getInstance();
    
    _input->setEngine(this);
}

void GameEngine::update(Engine* engine)
{
    _stateTime += FRAME_RATE;
    
    _timing->updateManual(_stateTime, FRAME_RATE);
    
    NG_CLIENT->processIncomingPackets();
    if (NG_CLIENT->getState() == NCS_Disconnected)
    {
        engine->getStateMachine().revertToPreviousState();
        return;
    }
    else if (NG_CLIENT->hasReceivedNewState())
    {
        uint32_t map = NG_CLIENT->getMap();
        if (_map != map)
        {
            _map = map;
            _world->loadMap(_map);
            _renderer->onNewMapLoaded();
        }
        
        _world->postRead();
    }
    
    NG_AUDIO_ENGINE->update();
    
    _input->update();
    if (handleNonMoveInput())
    {
        engine->getStateMachine().revertToPreviousState();
        return;
    }
    
    _world->updateClient();
    _input->clearPendingMove();
    NG_CLIENT->sendOutgoingPackets();
    
#ifdef NG_STEAM
    NG_STEAM_GAME_SERVICES->update(false);
#endif
    
    /// Only for host
    if (_server)
    {
        _server->update();
    }
}

void GameEngine::exit(Engine* engine)
{
    releaseDeviceDependentResources();
    
    GameInputManager::destroy();
    
    delete _world;
    InstanceManager::setClientWorld(NULL);
    
    Timing::getInstance()->updateManual(0, FRAME_RATE);
}

void GameEngine::createDeviceDependentResources()
{
    GM_CFG->initWithJsonFile("global.cfg");
    EntityMapper::getInstance()->initWithJsonFile("entities.cfg");
    EntityLayoutMapper::getInstance()->initWithJsonFile("maps.cfg");
    ASSETS->initWithJsonFile("game_assets.cfg");
    
    _renderer->createDeviceDependentResources();
    
    NGAudioEngine::create();
    
    NG_AUDIO_ENGINE->loadFromAssets();

    NG_AUDIO_ENGINE->setMusicDisabled(true);
    NG_AUDIO_ENGINE->setSoundsDisabled(true);
}

void GameEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight, GM_CFG->_framebufferWidth, GM_CFG->_framebufferHeight);
    
    CURSOR_CONVERTER->setCamSize(GM_CFG->_camWidth, GM_CFG->_camHeight);
    CURSOR_CONVERTER->setCursorSize(cursorWidth, cursorHeight);
}

void GameEngine::releaseDeviceDependentResources()
{
    _renderer->releaseDeviceDependentResources();
    
    NGAudioEngine::destroy();
}

void GameEngine::onResume()
{
    NG_AUDIO_ENGINE->resume();
}

void GameEngine::onPause()
{
    NG_AUDIO_ENGINE->pause();
}

void GameEngine::render(double alpha)
{
    if (_state & GameEngineState_Interpolation)
    {
        _world->interpolate(alpha);
    }
    
    _renderer->render();
    
    if (_state & GameEngineState_Interpolation)
    {
        _world->postRender();
    }
    
    NG_AUDIO_ENGINE->render();
}

bool GameEngine::handleNonMoveInput()
{
    int menuState = _input->getMenuState();
    switch (menuState)
    {
        case GIMS_LOCAL_PLAYER_DROP_OUT_0:
            return true;
        case GIMS_LOCAL_PLAYER_DROP_OUT_1:
            NG_CLIENT->requestToDropLocalPlayer(1);
            break;
        case GIMS_LOCAL_PLAYER_DROP_OUT_2:
            NG_CLIENT->requestToDropLocalPlayer(2);
            break;
        case GIMS_LOCAL_PLAYER_DROP_OUT_3:
            NG_CLIENT->requestToDropLocalPlayer(3);
            break;
        case GIMS_CLIENT_MAIN_TOGGLE_MUSIC:
            NG_AUDIO_ENGINE->setMusicDisabled(!NG_AUDIO_ENGINE->isMusicDisabled());
            break;
        case GIMS_CLIENT_MAIN_TOGGLE_SOUND:
            NG_AUDIO_ENGINE->setSoundsDisabled(!NG_AUDIO_ENGINE->areSoundsDisabled());
            break;
        case GIMS_TOGGLE_PHYSICS_DISPLAY:
            _state ^= GameEngineState_DisplayBox2D;
            break;
        case GIMS_TOGGLE_INTERPOLATION:
            _state ^= GameEngineState_Interpolation;
            break;
        case GIMS_TOGGLE_LIGHTING:
            _state ^= GameEngineState_Lighting;
            break;
        case GIMS_SERVER_TOGGLE_MAP:
            if (_server)
            {
                _server->toggleMap();
            }
            break;
        default:
        {
            GameInputState* inputState = _input->getInputState();
            if (inputState->isRequestingToAddLocalPlayer())
            {
                NG_CLIENT->requestToAddLocalPlayer();
            }
        }
            break;
    }
    
    return false;
}
