//
//  GameEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/GameEngine.h"

#include <game/graphics/portable/GameRenderer.h>
#include "game/logic/World.h"
#include "game/logic/GameInputManager.h"
#include "framework/util/Timing.h"
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
#include "game/logic/InstanceManager.h"
#include "game/logic/MainInputState.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "framework/entity/EntityManager.h"
#include "framework/network/client/SocketClientHelper.h"
#include "framework/network/portable/MachineAddress.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "framework/util/NGExtension.h"
#include "framework/util/PlatformHelper.h"
#include "framework/graphics/portable/Assets.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "framework/util/FPSUtil.h"
#include "framework/input/CursorConverter.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamClientHelper.h"
#include "framework/network/steam/NGSteamAddress.h"
#include "framework/network/steam/NGSteamGameServices.h"
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
_inputManager(NULL),
_timing(NULL),
_server(NULL),
_stateTime(0),
_state(GameEngineState_Default)
{
    _state |= GameEngineState_Interpolation;
}

GameEngine::~GameEngine()
{
    delete _renderer;
}

void GameEngine::enter(Engine* engine)
{
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getRenderWidth(), engine->getRenderHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    _stateTime = 0;
    
    GameInputManager::create();
    
    _world = InstanceManager::getClientWorld();
    _inputManager = GameInputManager::getInstance();
    _timing = Timing::getInstance();
    _server = Server::getInstance();
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
        _world->loadMapIfNecessary(NG_CLIENT->getMap());
        _world->postRead();
    }
    
    NG_AUDIO_ENGINE->update();
    
    _inputManager->update();
    if (handleNonMoveInput())
    {
        engine->getStateMachine().revertToPreviousState();
        return;
    }
    
    _world->update();
    _inputManager->clearPendingMove();
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
    
    Timing::getInstance()->updateManual(0, FRAME_RATE);
}

void GameEngine::createDeviceDependentResources()
{
    _renderer->createDeviceDependentResources();
    
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_ROBOT_JUMP, SOUND_ROBOT_JUMP, 4);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_EXPLOSION, SOUND_EXPLOSION, 8);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_DEATH, SOUND_DEATH, 2);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_FIRE_ROCKET, SOUND_FIRE_ROCKET, 8);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_ACTIVATE_SPRINT, SOUND_ACTIVATE_SPRINT, 4);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_HEADSHOT, SOUND_HEADSHOT, 4);
    
    NG_AUDIO_ENGINE->loadMusic(MUSIC_DEMO);
    
    if (PlatformHelper::getPlatform() != NG_PLATFORM_ANDROID
        && PlatformHelper::getPlatform() != NG_PLATFORM_IOS)
    {
        // No keyboard on mobile, meaning we can't hit the S key to turn on sound
        NG_AUDIO_ENGINE->setSoundDisabled(true);
        NG_AUDIO_ENGINE->setMusicDisabled(true);
    }
    else
    {
        NG_AUDIO_ENGINE->playMusic();
    }
}

void GameEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight, renderWidth, renderHeight);
    
    CURSOR_CONVERTER->setCamSize(CAM_WIDTH, CAM_HEIGHT);
    CURSOR_CONVERTER->setCursorSize(cursorWidth, cursorHeight);
}

void GameEngine::releaseDeviceDependentResources()
{
    _renderer->releaseDeviceDependentResources();
    
    NG_AUDIO_ENGINE->reset();
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
    
    _renderer->render(_state);
    
    if (_state & GameEngineState_Interpolation)
    {
        _world->postRender();
    }
}

bool GameEngine::handleNonMoveInput()
{
    int menuState = _inputManager->getMenuState();
    switch (menuState)
    {
        case GIS_LOCAL_PLAYER_DROP_OUT_0:
            return true;
        case GIS_LOCAL_PLAYER_DROP_OUT_1:
            NG_CLIENT->requestToDropLocalPlayer(1);
            break;
        case GIS_LOCAL_PLAYER_DROP_OUT_2:
            NG_CLIENT->requestToDropLocalPlayer(2);
            break;
        case GIS_LOCAL_PLAYER_DROP_OUT_3:
            NG_CLIENT->requestToDropLocalPlayer(3);
            break;
        case GIS_CLIENT_MAIN_TOGGLE_MUSIC:
            NG_AUDIO_ENGINE->setMusicDisabled(!NG_AUDIO_ENGINE->isMusicDisabled());
            break;
        case GIS_CLIENT_MAIN_TOGGLE_SOUND:
            NG_AUDIO_ENGINE->setSoundDisabled(!NG_AUDIO_ENGINE->isSoundDisabled());
            break;
        case GIS_TOGGLE_PHYSICS_DISPLAY:
        {
            if (_state & GameEngineState_DisplayBox2D)
            {
                _state &= ~GameEngineState_DisplayBox2D;
            }
            else
            {
                _state |= GameEngineState_DisplayBox2D;
            }
        }
            break;
        case GIS_TOGGLE_INTERPOLATION:
        {
            if (_state & GameEngineState_Interpolation)
            {
                _state &= ~GameEngineState_Interpolation;
            }
            else
            {
                _state |= GameEngineState_Interpolation;
            }
        }
            break;
        case GIS_SERVER_TOGGLE_SERVER_DISPLAY:
        {
            if (_server)
            {
                _server->toggleDisplaying();
            }
        }
            break;
        case GIS_SERVER_TOGGLE_MAP:
        {
            if (_server)
            {
                _server->toggleMap();
            }
        }
            break;
        default:
        {
            MainInputState* inputState = _inputManager->getInputState();
            if (inputState->isRequestingToAddLocalPlayer())
            {
                NG_CLIENT->requestToAddLocalPlayer();
            }
        }
            break;
    }
    
    return false;
}
