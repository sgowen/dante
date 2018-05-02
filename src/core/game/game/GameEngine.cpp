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
#include <framework/studio/World.h>
#include <framework/util/Timing.h>
#include <game/game/GameInputManager.h>
#include <game/game/Server.h>

#include <framework/util/Constants.h>
#include <framework/input/CursorInputManager.h>
#include <framework/input/CursorEvent.h>
#include <framework/input/CursorConverter.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <game/game/GameInputState.h>
#include <framework/entity/EntityManager.h>
#include <framework/network/client/SocketClientHelper.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/NGExtension.h>
#include <framework/util/PlatformHelper.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/Constants.h>
#include <framework/util/FPSUtil.h>
#include <framework/input/CursorConverter.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <game/config/GameConfig.h>
#include <game/game/Util.h>
#include <game/entity/PlayerController.h>
#include <framework/util/InstanceManager.h>
#include <framework/util/Config.h>
#include <framework/util/macros.h>

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

void GameEngine::sHandleTest(Engine* engine, uint32_t& testMap)
{
    Server::create(ServerFlag_TestSession, &testMap);
    assert(NG_SERVER);
    
    GameEngine* ge = GameEngine::getInstance();
    ge->_serverIPAddress = std::string("localhost:9999");
    ge->_name = std::string("TESTER");
    ge->_isHost = true;
    SET_BIT(ge->_state, GES_Host, true);
    SET_BIT(ge->_state, GES_Connected, false);
    
    engine->getStateMachine().changeState(ge);
}

void GameEngine::sHandleHostServer(Engine* engine, std::string inName)
{
    Server::create(0);
    assert(NG_SERVER);
    
    GameEngine* ge = GameEngine::getInstance();
    ge->_name = inName;
    ge->_isSteam = false;
    ge->_isHost = true;
    SET_BIT(ge->_state, GES_Host, true);
    SET_BIT(ge->_state, GES_Connected, false);
    
    engine->getStateMachine().changeState(ge);
}

void GameEngine::sHandleJoinServer(Engine* engine, std::string inServerIPAddress, std::string inName)
{
    GameEngine* ge = GameEngine::getInstance();
    ge->_serverIPAddress = inServerIPAddress;
    ge->_name = inName;
    ge->_isSteam = false;
    ge->_isHost = false;
    ge->joinServer();
    
    engine->getStateMachine().changeState(ge);
}

#ifdef NG_STEAM
void GameEngine::sHandleHostSteamServer(Engine* engine)
{
    Server::create(ServerFlag_Steam);
    assert(NG_SERVER);
    
    GameEngine* ge = GameEngine::getInstance();
    ge->_isSteam = true;
    ge->_isHost = true;
    SET_BIT(ge->_state, GES_Host, true);
    SET_BIT(ge->_state, GES_Connected, false);
    
    engine->getStateMachine().changeState(ge);
}

void GameEngine::sHandleJoinSteamServer(Engine* engine, CSteamID serverSteamID)
{
    GameEngine* ge = GameEngine::getInstance();
    ge->_serverSteamID = serverSteamID;
    ge->_isSteam = true;
    ge->_isHost = false;
    ge->joinServer();
    
    engine->getStateMachine().changeState(ge);
}
#endif

void GameEngine::enter(Engine* engine)
{
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    _map = 0;
    _world = new World(WorldFlag_Client);
    _timing->reset();
    _input = GameInputManager::getInstance();
    _server = Server::getInstance();
}

void GameEngine::update(Engine* engine)
{
#ifdef NG_STEAM
    if (NG_STEAM_GAME_SERVICES)
    {
        NG_STEAM_GAME_SERVICES->update();
    }
#endif
    
    if (_state & GES_Host &&
        !(_state & GES_Connected))
    {
        if (NG_SERVER && NG_SERVER->isConnected())
        {
            if (_isSteam)
            {
#ifdef NG_STEAM
                _serverSteamID = static_cast<NGSteamAddress*>(NG_SERVER->getServerAddress())->getSteamID();
#endif
            }
            else
            {
                _serverIPAddress = std::string("localhost:9999");
            }
            
            joinServer();
        }
        
        return;
    }
    
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
    
    if (_isHost)
    {
        _server->update();
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
    
    if (_server)
    {
        Server::destroy();
    }
    
    _timing->reset();
    
    SET_BIT(_state, GES_Host, false);
    SET_BIT(_state, GES_Connected, false);
}

void GameEngine::createDeviceDependentResources()
{
    GameInputManager::create();
    
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
    if (_state & GES_Interpolation)
    {
        for (Entity* entity : _world->getPlayers())
        {
            entity->interpolate(alpha);
        }
    }
    
    _renderer->render();
    
    if (_state & GES_Interpolation)
    {
        for (Entity* entity : _world->getPlayers())
        {
            entity->endInterpolation();
        }
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

void GameEngine::joinServer()
{
    if (_isSteam)
    {
#ifdef NG_STEAM
        NetworkManagerClient::create(new NGSteamClientHelper(_serverSteamID, GameEngine::sGetPlayerAddressHash, NG_CLIENT_CALLBACKS), GAME_ENGINE_CALLBACKS, INPUT_MANAGER_CALLBACKS);
#endif
    }
    else
    {
        NetworkManagerClient::create(new SocketClientHelper(_serverIPAddress, _name, FW_CFG->_clientPort, NG_CLIENT_CALLBACKS), GAME_ENGINE_CALLBACKS, INPUT_MANAGER_CALLBACKS);
    }
    
    assert(NG_CLIENT);
    
    SET_BIT(_state, GES_Connected, true);
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
_timing(static_cast<Timing*>(INSTANCE_MANAGER->get(INSTANCE_TIME_CLIENT))),
_input(NULL),
_state(GES_Default),
_map(0),
_isSteam(false),
_isHost(false),
_isLive(false)
{
    _state |= GES_Interpolation | GES_Lighting | GES_DisplayUI;
}

GameEngine::~GameEngine()
{
    delete _renderer;
}
