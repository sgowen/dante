//
//  GameEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/game/GameEngine.h>

#include <framework/entity/Entity.h>
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
#include <game/game/GameInputState.h>
#include <framework/entity/EntityManager.h>
#include <framework/network/client/SocketClientHelper.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/NGExtension.h>
#include <framework/util/PlatformHelper.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/Constants.h>
#include <framework/util/FPSUtil.h>
#include <framework/input/CursorConverter.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <game/logic/GameConfig.h>
#include <game/logic/Util.h>
#include <game/entity/PlayerController.h>
#include <framework/util/InstanceManager.h>

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

GameEngine* GameEngine::getInstance()
{
    return s_instance;
}

void GameEngine::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

uint64_t GameEngine::sGetPlayerAddressHash(uint8_t inPlayerIndex)
{
    GameEngine* engine = GameEngine::getInstance();
    assert(engine);
    
    World* world = engine->_world;
    assert(world);
    
    uint64_t ret = 0;
    Entity* entity = world->getPlayerWithId(inPlayerIndex + 1);
    if (entity)
    {
        PlayerController* robot = static_cast<PlayerController*>(entity->getController());
        assert(robot);
        
        ret = robot->getAddressHash();
    }
    
    return ret;
}

void GameEngine::sHandleDynamicEntityCreatedOnClient(Entity* inEntity)
{
    GameEngine* engine = GameEngine::getInstance();
    assert(engine);
    
    World* world = engine->_world;
    assert(world);
    
    world->addDynamicEntity(inEntity);
}

void GameEngine::sHandleDynamicEntityDeletedOnClient(Entity* inEntity)
{
    GameEngine* engine = GameEngine::getInstance();
    assert(engine);
    
    World* world = engine->_world;
    assert(world);
    
    world->removeDynamicEntity(inEntity);
}

GameEngine::GameEngine() : EngineState(),
_renderer(new GameRenderer()),
_world(NULL),
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
    _world = new World();
    _timing = static_cast<Timing*>(INSTANCE_MANAGER->getInstance(INSTANCE_TIME_CLIENT));
    
    GameInputManager::create(this);
}

void GameEngine::update(Engine* engine)
{
    _stateTime += FRAME_RATE;
    
    _timing->setTime(_stateTime);
    
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
    
    GameInputManager::getInstance()->update();
    if (GameInputManager::getInstance()->getMenuState() == GIMS_ESCAPE)
    {
        engine->getStateMachine().revertToPreviousState();
        return;
    }
    
    _world->updateClient();
    GameInputManager::getInstance()->clearPendingMove();
    NG_CLIENT->sendOutgoingPackets();
    
#ifdef NG_STEAM
    NG_STEAM_GAME_SERVICES->update(false);
#endif
    
    if (Server::getInstance())
    {
        /// Only for host
        Server::getInstance()->update();
    }
}

void GameEngine::exit(Engine* engine)
{
    releaseDeviceDependentResources();
    
    GameInputManager::destroy();
    
    delete _world;
    
    if (NG_CLIENT)
    {
        NetworkManagerClient::destroy();
    }
    
    if (Server::getInstance())
    {
        Server::destroy();
    }
    
    _timing->setTime(0);
}

void GameEngine::createDeviceDependentResources()
{
    GM_CFG->initWithJsonFile("global.cfg");
    EntityMapper::getInstance()->initWithJsonFile("entities.cfg");
    EntityLayoutMapper::getInstance()->initWithJsonFile("maps.cfg");
    ASSETS->initWithJsonFile("game_assets.cfg");
    
    CURSOR_CONVERTER->setCamSize(GM_CFG->_camWidth, GM_CFG->_camHeight);
    
    _renderer->createDeviceDependentResources();
    
    NGAudioEngine::create();
    NG_AUDIO_ENGINE->loadFromAssets();
    NG_AUDIO_ENGINE->setMusicDisabled(true);
    NG_AUDIO_ENGINE->setSoundsDisabled(true);
}

void GameEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight);
    
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
