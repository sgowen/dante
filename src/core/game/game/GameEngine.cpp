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
#include <framework/util/Timing.h>
#include <game/game/GameInputManager.h>

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
#include <framework/util/Config.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamClientHelper.h>
#include <framework/network/steam/NGSteamAddress.h>
#include <framework/network/steam/NGSteamGameServices.h>
#endif

IMPL_RTTI(GameEngine, EngineState);

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
    uint64_t ret = 0;
    
    GameEngine* engine = GameEngine::getInstance();
    assert(engine);
    
    World* world = engine->_world;
    assert(world);
    
    uint8_t playerID = inPlayerIndex + 1;
    
    Entity* player = NULL;
    for (Entity* e : world->getPlayers())
    {
        PlayerController* robot = static_cast<PlayerController*>(e->getController());
        if (robot->getPlayerId() == playerID)
        {
            player = e;
            break;
        }
    }
    
    if (player)
    {
        PlayerController* robot = static_cast<PlayerController*>(player->getController());
        assert(robot);
        
        ret = robot->getAddressHash();
    }
    
    return ret;
}

void GameEngine::sHandleDynamicEntityCreatedOnClient(Entity* entity)
{
    GameEngine* engine = GameEngine::getInstance();
    assert(engine);
    
    World* world = engine->_world;
    assert(world);
    
    world->addEntity(entity);
}

void GameEngine::sHandleDynamicEntityDeletedOnClient(Entity* entity)
{
    GameEngine* engine = GameEngine::getInstance();
    assert(engine);
    
    World* world = engine->_world;
    assert(world);
    
    world->removeEntity(entity);
}

void GameEngine::enter(Engine* engine)
{
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    _map = 0;
    _world = new World();
    _timing->reset();
    
    GameInputManager::create(this);
    _input = GameInputManager::getInstance();
}

void GameEngine::update(Engine* engine)
{
    _timing->onFrame();
    
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
        
        for (Entity* e : _world->getDynamicEntities())
        {
            e->getNetworkController()->recallNetworkCache();
        }
        
        // all processed moves have been removed, so all that are left are unprocessed moves so we must apply them...
        for (const Move& move : _input->getMoveList())
        {
            updateWorld(&move);
        }
    }
    
    NG_AUDIO_ENGINE->update();
    
    _input->update();
    if (_input->getMenuState() == GIMS_ESCAPE)
    {
        engine->getStateMachine().revertToPreviousState();
        return;
    }
    
    _isLive = true;
    updateWorld(_input->getPendingMove());
    _isLive = false;
    
    _input->clearPendingMove();
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
    _input = NULL;
    
    delete _world;
    
    if (NG_CLIENT)
    {
        NetworkManagerClient::destroy();
    }
    
    if (Server::getInstance())
    {
        Server::destroy();
    }
    
    _timing->reset();
}

void GameEngine::createDeviceDependentResources()
{
    GM_CFG->initWithJsonFile("global.cfg");
    EntityMapper::getInstance()->initWithJsonFile("entities.cfg");
    EntityLayoutMapper::getInstance()->initWithJsonFile("maps.cfg");
    ASSETS->initWithJsonFile("game_assets.cfg");
    
    CURSOR_CONVERTER->setCamSize(FW_CFG->_camWidth, FW_CFG->_camHeight);
    
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
        _world->endInterpolation();
    }
    
    NG_AUDIO_ENGINE->render();
}

World* GameEngine::getWorld()
{
    return _world;
}

bool GameEngine::isLive()
{
    return _isLive;
}

void GameEngine::updateWorld(const Move* move)
{
    assert(move);
    
    for (Entity* e : _world->getPlayers())
    {
        PlayerController* robot = static_cast<PlayerController*>(e->getController());
        robot->processInput(move->getInputState());
    }
    
    _world->stepPhysics();
    
    for (Entity* e : _world->getDynamicEntities())
    {
        e->update();
    }
}

GameEngine::GameEngine() : EngineState(),
_renderer(new GameRenderer()),
_world(NULL),
_timing(static_cast<Timing*>(INSTANCE_MANAGER->getInstance(INSTANCE_TIME_CLIENT))),
_input(NULL),
_state(GameEngineState_Default),
_map(0),
_isLive(false)
{
    _state |= GameEngineState_Interpolation | GameEngineState_Lighting | GameEngineState_DisplayUI;
    
    _renderer->setEngine(this);
}

GameEngine::~GameEngine()
{
    delete _renderer;
}
