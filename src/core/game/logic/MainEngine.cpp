//
//  MainEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/MainEngine.h"

#include "game/network/Server.h"
#include "framework/file/portable/JsonFile.h"
#include "framework/entity/Entity.h"

#include "game/logic/GameConstants.h"
#include "framework/input/CursorInputManager.h"
#include "framework/input/CursorEvent.h"
#include "framework/input/CursorConverter.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include "game/logic/Robot.h"
#include "game/logic/Projectile.h"
#include "game/logic/SpacePirate.h"
#include "framework/entity/EntityRegistry.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/portable/SocketAddressFactory.h"
#include "framework/network/portable/SocketUtil.h"
#include "game/logic/InputManager.h"
#include "game/logic/World.h"
#include "game/network/InstanceManager.h"
#include "game/network/MainInputState.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "framework/entity/EntityManager.h"
#include "framework/network/client/SocketClientHelper.h"
#include "framework/network/portable/MachineAddress.h"
#include "game/logic/MainEngineState.h"
#include "game/graphics/portable/MainRenderer.h"
#include "framework/audio/portable/NGAudioEngine.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamClientHelper.h"
#include "framework/network/steam/NGSteamAddress.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

MainEngine::MainEngine() : Engine(new MainRenderer(MAX_BATCH_SIZE)),
_config(new JsonFile("dante.cfg")),
_isSteam(false)
{
    CURSOR_CONVERTER->setCamSize(CAM_WIDTH, CAM_HEIGHT);
    
    activateSteam();
    
    _config->load();
    
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_Robot, World::sClientCreateRobot);
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_Projectile, World::sClientCreateProjectile);
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_SpacePirate, World::sClientCreateSpacePirate);
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_Crate, World::sClientCreateCrate);
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_SpacePirateChunk, World::sClientCreateSpacePirateChunk);
}

MainEngine::~MainEngine()
{
    delete _config;
    
    disconnect();
	deactivateSteam();
}

void MainEngine::createDeviceDependentResources()
{
    Engine::createDeviceDependentResources();
    
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_ROBOT_JUMP, SOUND_ROBOT_JUMP, 4);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_EXPLOSION, SOUND_EXPLOSION, 8);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_DEATH, SOUND_DEATH, 2);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_FIRE_ROCKET, SOUND_FIRE_ROCKET, 8);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_ACTIVATE_SPRINT, SOUND_ACTIVATE_SPRINT, 4);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_HEADSHOT, SOUND_HEADSHOT, 4);
    
    NG_AUDIO_ENGINE->loadMusic(MUSIC_DEMO);
    
    NG_AUDIO_ENGINE->setSoundDisabled(true);
    NG_AUDIO_ENGINE->setMusicDisabled(true);
}

void MainEngine::releaseDeviceDependentResources()
{
    Engine::releaseDeviceDependentResources();
    
    NG_AUDIO_ENGINE->reset();
}

void MainEngine::update(float deltaTime)
{
    Engine::update(deltaTime);
    
    if (Server::getInstance())
    {
        Server::getInstance()->update(deltaTime);
    }
}

void MainEngine::onFrame()
{
    handleSteamGameServices();
    
    if (NG_CLIENT)
    {
        NG_CLIENT->processIncomingPackets();
        
        InstanceManager::getClientWorld()->postRead();
    }
    
    InputManager::getInstance()->update();
    
    NG_AUDIO_ENGINE->update();
    
    if (InstanceManager::getClientWorld())
    {
        InstanceManager::getClientWorld()->update();
    }
    
    InputManager::getInstance()->clearPendingMove();
    
    handleNonMoveInput();
    
    if (NG_CLIENT)
    {
        NG_CLIENT->sendOutgoingPackets();
        
        if (NG_CLIENT->getState() == NCS_Disconnected)
        {
            disconnect();
        }
    }
}

void MainEngine::handleNonMoveInput()
{
    MainInputState* inputState = InputManager::getInstance()->getInputState();
    
    if (inputState->getMenuState() == MENU_STATE_CLIENT_MAIN_TOGGLE_MUSIC)
    {
        NG_AUDIO_ENGINE->setMusicDisabled(!NG_AUDIO_ENGINE->isMusicDisabled());
    }
    else if (inputState->getMenuState() == MENU_STATE_CLIENT_MAIN_TOGGLE_SOUND)
    {
        NG_AUDIO_ENGINE->setSoundDisabled(!NG_AUDIO_ENGINE->isSoundDisabled());
    }
    
    if (NG_CLIENT)
    {
        if (Server::getInstance())
        {
            if (inputState->getMenuState() == MENU_STATE_SERVER_TOGGLE_ENEMIES)
            {
                Server::getInstance()->toggleEnemies();
            }
            else if (inputState->getMenuState() == MENU_STATE_SERVER_TOGGLE_OBJECTS)
            {
                Server::getInstance()->toggleObjects();
            }
            else if (inputState->getMenuState() == MENU_STATE_SERVER_TOGGLE_SERVER_DISPLAY)
            {
                Server::getInstance()->toggleDisplaying();
            }
        }
        
        if (inputState->isRequestingToAddLocalPlayer())
        {
            NG_CLIENT->requestToAddLocalPlayer();
        }
        else if (inputState->getMenuState() == MENU_STATE_ESCAPE)
        {
            disconnect();
        }
        else if (inputState->getMenuState() == MENU_STATE_LOCAL_PLAYER_DROP_OUT_1)
        {
            NG_CLIENT->requestToDropLocalPlayer(1);
        }
        else if (inputState->getMenuState() == MENU_STATE_LOCAL_PLAYER_DROP_OUT_2)
        {
            NG_CLIENT->requestToDropLocalPlayer(2);
        }
        else if (inputState->getMenuState() == MENU_STATE_LOCAL_PLAYER_DROP_OUT_3)
        {
            NG_CLIENT->requestToDropLocalPlayer(3);
        }
    }
    else if (NG_SERVER)
    {
        if (inputState->getMenuState() == MENU_STATE_ESCAPE)
        {
            disconnect();
        }
        else if (_isSteam)
        {
            if (NG_SERVER->isConnected())
            {
#ifdef NG_STEAM
                _serverSteamID = static_cast<NGSteamAddress*>(NG_SERVER->getServerAddress())->getSteamID();
                joinServer();
#endif
            }
        }
        else
        {
            if (NG_SERVER->isConnected())
            {
                _serverIPAddress = std::string("localhost:9999");
                joinServer();
            }
        }
    }
    else if (InputManager::getInstance()->isLiveMode())
    {
        if (inputState->getMenuState() == MENU_STATE_ESCAPE)
        {
            InputManager::getInstance()->setLiveMode(false);
            InputManager::getInstance()->resetLiveInput();
            _engineState = _isSteam ? MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON : MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
        }
        else if (InputManager::getInstance()->isTimeToProcessInput())
        {
            if (_engineState == MAIN_ENGINE_STATE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP)
            {
                _serverIPAddress = StringUtil::format("%s:%d", InputManager::getInstance()->getLiveInput().c_str(), SERVER_PORT);
                _name.clear();
                _engineState = MAIN_ENGINE_STATE_MAIN_MENU_ENTERING_USERNAME;
            }
            else if (_engineState == MAIN_ENGINE_STATE_MAIN_MENU_ENTERING_USERNAME)
            {
                _name = InputManager::getInstance()->getLiveInput();
                InputManager::getInstance()->setLiveMode(false);
                
                if (_serverIPAddress.length() == 0)
                {
                    startServer();
                }
                else
                {
                    joinServer();
                }
            }
            
            InputManager::getInstance()->resetLiveInput();
        }
    }
    else
    {
        if (inputState->getMenuState() == MENU_STATE_ACTIVATE_STEAM)
        {
            activateSteam();
        }
        else if (inputState->getMenuState() == MENU_STATE_DEACTIVATE_STEAM)
        {
            deactivateSteam();
        }
        else if (inputState->getMenuState() == MENU_STATE_START_SERVER)
        {
            if (_isSteam)
            {
                startServer();
            }
            else
            {
                _serverIPAddress.clear();
                _name.clear();
                _engineState = MAIN_ENGINE_STATE_MAIN_MENU_ENTERING_USERNAME;
                InputManager::getInstance()->setLiveMode(true);
            }
        }
        else if (inputState->getMenuState() == MENU_STATE_JOIN_LOCAL_SERVER)
        {
            if (!_isSteam)
            {
                _serverIPAddress.clear();
                _engineState = MAIN_ENGINE_STATE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP;
                InputManager::getInstance()->setLiveMode(true);
            }
        }
        else if (inputState->getMenuState() == MENU_STATE_STEAM_REFRESH_LAN_SERVERS)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES)
            {
                NG_STEAM_GAME_SERVICES->refreshLANServers();
            }
#endif
        }
        else if (inputState->getMenuState() == MENU_STATE_STEAM_REFRESH_INTERNET_SERVERS)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES)
            {
                NG_STEAM_GAME_SERVICES->refreshInternetServers();
            }
#endif
        }
        else if (inputState->getMenuState() == MENU_STATE_STEAM_JOIN_SERVER_1
                 || inputState->getMenuState() == MENU_STATE_STEAM_JOIN_SERVER_2
                 || inputState->getMenuState() == MENU_STATE_STEAM_JOIN_SERVER_3
                 || inputState->getMenuState() == MENU_STATE_STEAM_JOIN_SERVER_4)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES && !NG_STEAM_GAME_SERVICES->isRequestingServers())
            {
                int serverIndex = inputState->getMenuState() - 7; // eh, hacky I know, but whatever
                std::vector<NGSteamGameServer> gameServers = NG_STEAM_GAME_SERVICES->getGameServers();
                if (gameServers.size() > serverIndex)
                {
                    NG_STEAM_GAME_SERVICES->initiateServerConnection(gameServers[serverIndex].getSteamID());
                }
            }
#endif
        }
        else if (inputState->getMenuState() == MENU_STATE_ESCAPE)
        {
            _requestedAction = REQUESTED_ACTION_EXIT;
        }
    }
}

void MainEngine::activateSteam()
{
#ifdef NG_STEAM
    if (!NGSteamGameServices::getInstance())
    {
        NGSteamGameServices::create(STEAM_GAME_DIR);
    }
    
    _isSteam = NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS;
    _engineState = _isSteam ? MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON : MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
#endif
}

void MainEngine::handleSteamGameServices()
{
#ifdef NG_STEAM
    if (NG_STEAM_GAME_SERVICES)
    {
        NG_STEAM_GAME_SERVICES->update(NG_SERVER ? true : false);
        
        if (NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS)
        {
            if (NG_STEAM_GAME_SERVICES->isRequestingToJoinServer())
            {
                disconnect();
                _serverSteamID = NG_STEAM_GAME_SERVICES->getServerToJoinSteamID();
                joinServer();
            }
        }
        else
        {
            disconnect();
            deactivateSteam();
        }
    }
#endif
}

void MainEngine::deactivateSteam()
{
#ifdef NG_STEAM
    if (NGSteamGameServices::getInstance())
    {
        NGSteamGameServices::destroy();
    }
    
    _isSteam = false;
    _engineState = MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
#endif
}

void MainEngine::startServer()
{
    _engineState = MAIN_ENGINE_STATE_MAIN_MENU_STARTING_SERVER;
    
    if (!Server::getInstance())
    {
        int numCratesToSpawn = -1;
        {
            std::string key = std::string("nu_crates_to_spawn");
            std::string val = _config->findValue(key);
            if (val.length() > 0)
            {
                numCratesToSpawn = StringUtil::stringToNumber<int>(val);
            }
        }
        
        int numSpacePiratesToSpawn = -1;
        {
            std::string key = std::string("nu_space_pirates_to_spawn");
            std::string val = _config->findValue(key);
            if (val.length() > 0)
            {
                numSpacePiratesToSpawn = StringUtil::stringToNumber<int>(val);
            }
        }
        
        if (numCratesToSpawn > -1 && numSpacePiratesToSpawn > -1)
        {
            Server::create(_isSteam, numCratesToSpawn, numSpacePiratesToSpawn);
        }
        else
        {
            Server::create(_isSteam);
        }
        
        if (!NG_SERVER)
        {
            Server::destroy();
        }
    }
}

void MainEngine::joinServer()
{
    _engineState = MAIN_ENGINE_STEAM_JOINING_SERVER;
    
    FWInstanceManager::createClientEntityManager(InstanceManager::sHandleEntityCreatedOnClient, InstanceManager::sHandleEntityDeletedOnClient);
    
    InstanceManager::createClientWorld();
    
    ClientHelper* clientHelper = NULL;
    if (_isSteam)
    {
#ifdef NG_STEAM
        clientHelper = new NGSteamClientHelper(_serverSteamID, InstanceManager::sGetPlayerAddressHashForIndexOnClient, NG_CLIENT_CALLBACKS);
#endif
    }
    else
    {
        clientHelper = new SocketClientHelper(_serverIPAddress, _name, CLIENT_PORT, NG_CLIENT_CALLBACKS);
    }
    
    NetworkManagerClient::create(clientHelper, FRAME_RATE, INPUT_MANAGER_CALLBACKS);
    
    InputManager::getInstance()->setConnected(NG_CLIENT ? true : false);
}

void MainEngine::disconnect()
{
    if (NG_CLIENT)
    {
        NetworkManagerClient::destroy();
        
        FWInstanceManager::destroyClientEntityManager();
        
        InstanceManager::destroyClientWorld();
    }
    
    if (Server::getInstance())
    {
        Server::destroy();
    }
    
    _engineState = _isSteam ? MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON : MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
    
    InputManager::getInstance()->setConnected(false);
}

RTTI_IMPL(MainEngine, Engine);
