//
//  TitleEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/TitleEngine.h"

#include "framework/file/portable/JsonFile.h"
#include <game/graphics/portable/TitleRenderer.h>

#include "game/logic/Server.h"
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
#include "game/logic/TitleInputManager.h"
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
#include "game/logic/GameEngine.h"
#include "game/logic/GameInputManager.h"
#include "game/logic/StudioEngine.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamClientHelper.h"
#include "framework/network/steam/NGSteamAddress.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

NGRTTI_IMPL(TitleEngine, EngineState);

TitleEngine* TitleEngine::s_pInstance = NULL;

void TitleEngine::create()
{
    assert(!s_pInstance);
    
    s_pInstance = new TitleEngine();
}

TitleEngine * TitleEngine::getInstance()
{
    return s_pInstance;
}

void TitleEngine::destroy()
{
    assert(s_pInstance);
    
    delete s_pInstance;
    s_pInstance = NULL;
}

TitleEngine::TitleEngine() : EngineState(),
_config(new JsonFile("dante.cfg")),
_renderer(new TitleRenderer()),
_isSteam(false),
_state(TE_MAIN_MENU_STEAM_OFF)
{
    NoctisGames::NGExtension::setInstance(NoctisGames::DefaultNGExtension::getInstance());
    
    activateSteam();
}

TitleEngine::~TitleEngine()
{
    delete _config;
    delete _renderer;
    
	deactivateSteam();
}

void TitleEngine::enter(Engine* engine)
{
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getRenderWidth(), engine->getRenderHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    disconnect();
}

void TitleEngine::update(Engine* engine)
{
    handleSteamGameServices(engine);
    
    TitleInputManager::getInstance()->update();
    
    if (handleInput(engine))
    {
        return;
    }
    
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
        
        joinServer(engine);
    }
}

void TitleEngine::exit(Engine* engine)
{
    releaseDeviceDependentResources();
}

void TitleEngine::createDeviceDependentResources()
{
    _renderer->createDeviceDependentResources();
    
    _config->load();
}

void TitleEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight, renderWidth, renderHeight);
    
    CURSOR_CONVERTER->setCamSize(CAM_WIDTH, CAM_HEIGHT);
    CURSOR_CONVERTER->setCursorSize(cursorWidth, cursorHeight);
}

void TitleEngine::releaseDeviceDependentResources()
{
    _renderer->releaseDeviceDependentResources();
}

void TitleEngine::onResume()
{
    // Empty
}

void TitleEngine::onPause()
{
    // Empty
}

void TitleEngine::render()
{
    _renderer->render(_state);
}

bool TitleEngine::handleInput(Engine* engine)
{
    int menuState = TitleInputManager::getInstance()->getMenuState();
    
    if (TitleInputManager::getInstance()->isLiveMode())
    {
        if (menuState == MIS_ESCAPE)
        {
            TitleInputManager::getInstance()->setLiveInputMode(false);
            
            _state = _isSteam ? TE_MAIN_MENU_STEAM_ON : TE_MAIN_MENU_STEAM_OFF;
        }
        else if (TitleInputManager::getInstance()->isTimeToProcessInput())
        {
            if (_state == TE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP)
            {
                _serverIPAddress = StringUtil::format("%s:%d", TitleInputManager::getInstance()->getLiveInput().c_str(), SERVER_PORT);
                _name.clear();
                _state = TE_MAIN_MENU_ENTERING_USERNAME;
            }
            else if (_state == TE_MAIN_MENU_ENTERING_USERNAME)
            {
                _name = TitleInputManager::getInstance()->getLiveInput();
                TitleInputManager::getInstance()->setLiveInputMode(false);
                
                if (_serverIPAddress.length() == 0)
                {
                    startServer();
                }
                else
                {
                    joinServer(engine);
                }
            }
            
            TitleInputManager::getInstance()->onInputProcessed();
        }
    }
    else
    {
        if (menuState == MIS_ENTER_STUDIO)
        {
#ifndef NG_MOBILE
            engine->getStateMachine().changeState(StudioEngine::getInstance());
            return true;
#endif
        }
        else if (menuState == MIS_ACTIVATE_STEAM)
        {
            activateSteam();
        }
        else if (menuState == MIS_DEACTIVATE_STEAM)
        {
            deactivateSteam();
        }
        else if (menuState == MIS_START_SERVER)
        {
            if (_isSteam)
            {
                startServer();
            }
            else
            {
                _serverIPAddress.clear();
                _name.clear();
                _state = TE_MAIN_MENU_ENTERING_USERNAME;
                TitleInputManager::getInstance()->setLiveInputMode(true);
            }
        }
        else if (menuState == MIS_JOIN_LOCAL_SERVER)
        {
            if (!_isSteam)
            {
                _serverIPAddress.clear();
                _state = TE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP;
                TitleInputManager::getInstance()->setLiveInputMode(true);
            }
        }
        else if (menuState == MIS_STEAM_REFRESH_LAN_SERVERS)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES)
            {
                NG_STEAM_GAME_SERVICES->refreshLANServers();
            }
#endif
        }
        else if (menuState == MIS_STEAM_REFRESH_INTERNET_SERVERS)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES)
            {
                NG_STEAM_GAME_SERVICES->refreshInternetServers();
            }
#endif
        }
        else if (menuState == MIS_STEAM_JOIN_SERVER_1
                 || menuState == MIS_STEAM_JOIN_SERVER_2
                 || menuState == MIS_STEAM_JOIN_SERVER_3
                 || menuState == MIS_STEAM_JOIN_SERVER_4)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES && !NG_STEAM_GAME_SERVICES->isRequestingServers())
            {
                int serverIndex = menuState - 7; // eh, hacky I know, but whatever
                std::vector<NGSteamGameServer> gameServers = NG_STEAM_GAME_SERVICES->getGameServers();
                if (gameServers.size() > serverIndex)
                {
                    NG_STEAM_GAME_SERVICES->initiateServerConnection(gameServers[serverIndex].getSteamID());
                }
            }
#endif
        }
        else if (menuState == MIS_ESCAPE)
        {
            engine->setRequestedAction(REQUESTED_ACTION_EXIT);
            return true;
        }
    }
    
    return false;
}

void TitleEngine::activateSteam()
{
    _state = TE_MAIN_MENU_STEAM_OFF;
    
#ifdef NG_STEAM
    if (!NGSteamGameServices::getInstance())
    {
        NGSteamGameServices::create(STEAM_GAME_DIR);
    }
    
    _isSteam = NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS;
    _state = _isSteam ? TE_MAIN_MENU_STEAM_ON : TE_MAIN_MENU_STEAM_OFF;
#endif
}

void TitleEngine::handleSteamGameServices(Engine* engine)
{
#ifdef NG_STEAM
    if (NG_STEAM_GAME_SERVICES)
    {
        NG_STEAM_GAME_SERVICES->update(false);
        NG_STEAM_GAME_SERVICES->update(true);
        
        if (NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS)
        {
            if (NG_STEAM_GAME_SERVICES->isRequestingToJoinServer())
            {
                disconnect();
                _serverSteamID = NG_STEAM_GAME_SERVICES->getServerToJoinSteamID();
                joinServer(engine);
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

void TitleEngine::deactivateSteam()
{
#ifdef NG_STEAM
    if (NGSteamGameServices::getInstance())
    {
        NGSteamGameServices::destroy();
    }
    
    _isSteam = false;
    _state = TE_MAIN_MENU_STEAM_OFF;
#endif
}

void TitleEngine::startServer()
{
    _state = TE_MAIN_MENU_STARTING_SERVER;
    
    if (!Server::getInstance())
    {
        uint32_t numCratesToSpawn = 6;
        uint32_t numSpacePiratesToSpawn = 4;
        
        {
            std::string val = _config->findValue(std::string("nu_crates_to_spawn"));
            if (val.length() > 0)
            {
                numCratesToSpawn = StringUtil::stringToNumber<int>(val);
            }
        }
        
        {
            std::string val = _config->findValue(std::string("nu_space_pirates_to_spawn"));
            if (val.length() > 0)
            {
                numSpacePiratesToSpawn = StringUtil::stringToNumber<int>(val);
            }
        }
        
        Server::create(_isSteam, numCratesToSpawn, numSpacePiratesToSpawn);
        
        if (PlatformHelper::getPlatform() == NG_PLATFORM_ANDROID
            || PlatformHelper::getPlatform() == NG_PLATFORM_IOS)
        {
            Server::getInstance()->toggleEnemies();
            Server::getInstance()->toggleObjects();
        }
        
        if (!NG_SERVER)
        {
            Server::destroy();
        }
    }
}

void TitleEngine::joinServer(Engine* engine)
{
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
    
    assert(clientHelper);
    
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_Robot, World::sClientCreateRobot);
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_Projectile, World::sClientCreateProjectile);
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_SpacePirate, World::sClientCreateSpacePirate);
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_Crate, World::sClientCreateCrate);
    CLIENT_ENTITY_REG->registerCreationFunction(NW_TYPE_SpacePirateChunk, World::sClientCreateSpacePirateChunk);
    
    NetworkManagerClient::create(clientHelper, INPUT_MANAGER_CALLBACKS);
    
    assert(NG_CLIENT);
    
    engine->getStateMachine().changeState(GameEngine::getInstance());
}

void TitleEngine::disconnect()
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
    
    _state = _isSteam ? TE_MAIN_MENU_STEAM_ON : TE_MAIN_MENU_STEAM_OFF;
}
