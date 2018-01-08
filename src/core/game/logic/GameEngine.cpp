//
//  GameEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/GameEngine.h"

#include "framework/file/portable/JsonFile.h"
#include <game/graphics/portable/GameRenderer.h>

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
#include "game/logic/GameInputManager.h"
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
_stateTime(0),
_state(GE_DEFAULT)
{
    // Empty
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
    _state = GE_DEFAULT;
    
    GameInputManager::create();
}

void GameEngine::update(Engine* engine)
{
    _stateTime += FRAME_RATE;
    
    Timing::getInstance()->updateManual(_stateTime, FRAME_RATE);
    
    NG_CLIENT->processIncomingPackets();
    
    InstanceManager::getClientWorld()->postRead();
    
    GameInputManager::getInstance()->update();
    
    NG_AUDIO_ENGINE->update();
    
    InstanceManager::getClientWorld()->update();
    
    GameInputManager::getInstance()->clearPendingMove();
    
    NG_CLIENT->sendOutgoingPackets();
    
    if (NG_CLIENT->getState() == NCS_Disconnected
        || handleNonMoveInput())
    {
        engine->getStateMachine().revertToPreviousState();
        return;
    }
    
#ifdef NG_STEAM
    NG_STEAM_GAME_SERVICES->update(false);
#endif
    
    if (Server::getInstance())
    {
        Server::getInstance()->update();
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

void GameEngine::render()
{
    _renderer->render(_state);
}

bool GameEngine::handleNonMoveInput()
{
    int menuState = GameInputManager::getInstance()->getMenuState();
    
    if (menuState == MIS_CLIENT_MAIN_TOGGLE_MUSIC)
    {
        NG_AUDIO_ENGINE->setMusicDisabled(!NG_AUDIO_ENGINE->isMusicDisabled());
    }
    else if (menuState == MIS_CLIENT_MAIN_TOGGLE_SOUND)
    {
        NG_AUDIO_ENGINE->setSoundDisabled(!NG_AUDIO_ENGINE->isSoundDisabled());
    }
    
    if (Server::getInstance())
    {
        if (menuState == MIS_SERVER_TOGGLE_OBJECTS)
        {
            Server::getInstance()->toggleObjects();
        }
        else if (menuState == MIS_SERVER_TOGGLE_SERVER_DISPLAY)
        {
            Server::getInstance()->toggleDisplaying();
        }
        else if (menuState == MIS_SERVER_TOGGLE_PHYSICS_DISPLAY)
        {
            if (isFlagSet(_state, GE_DISPLAY_BOX_2D))
            {
                removeFlag(_state, GE_DISPLAY_BOX_2D);
            }
            else
            {
                setFlag(_state, GE_DISPLAY_BOX_2D);
            }
        }
    }
    
    MainInputState* inputState = GameInputManager::getInstance()->getInputState();
    if (inputState->isRequestingToAddLocalPlayer())
    {
        NG_CLIENT->requestToAddLocalPlayer();
    }
    else if (menuState == MIS_ESCAPE)
    {
        return true;
    }
    else if (menuState == MIS_LOCAL_PLAYER_DROP_OUT_1)
    {
        NG_CLIENT->requestToDropLocalPlayer(1);
    }
    else if (menuState == MIS_LOCAL_PLAYER_DROP_OUT_2)
    {
        NG_CLIENT->requestToDropLocalPlayer(2);
    }
    else if (menuState == MIS_LOCAL_PLAYER_DROP_OUT_3)
    {
        NG_CLIENT->requestToDropLocalPlayer(3);
    }
    
    return false;
}
