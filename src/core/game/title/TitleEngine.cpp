//
//  TitleEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/title/TitleEngine.h>

#include <framework/file/portable/JsonFile.h>
#include <game/title/TitleRenderer.h>

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
#include <game/title/TitleInputManager.h>
#include <game/logic/World.h>
#include <game/game/GameInputState.h>
#include <framework/entity/EntityManager.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
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
#include <game/game/GameEngine.h>
#include <game/game/GameInputManager.h>
#include <game/studio/StudioEngine.h>
#include <game/logic/GameConfig.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamClientHelper.h>
#include <framework/network/steam/NGSteamAddress.h>
#include <framework/network/steam/NGSteamGameServices.h>
#endif

NGRTTI_IMPL(TitleEngine, EngineState);

TitleEngine* TitleEngine::s_instance = NULL;

void TitleEngine::create()
{
    assert(!s_instance);
    
    s_instance = new TitleEngine();
}

TitleEngine * TitleEngine::getInstance()
{
    return s_instance;
}

void TitleEngine::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

TitleEngine::TitleEngine() : EngineState(),
_renderer(new TitleRenderer()),
_isSteam(false),
_state(TitleEngineState_SteamOff)
{
    NoctisGames::NGExtension::setInstance(NoctisGames::DefaultNGExtension::getInstance());
    
    activateSteam();
    
    _renderer->setEngine(this);
}

TitleEngine::~TitleEngine()
{
    delete _renderer;
    
	deactivateSteam();
}

void TitleEngine::enter(Engine* engine)
{
    disconnect();
    
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    TitleInputManager::getInstance()->setEngine(this);
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
    GM_CFG->initWithJsonFile("global.cfg");
    EntityMapper::getInstance()->initWithJsonFile("entities.cfg");
    EntityLayoutMapper::getInstance()->initWithJsonFile("maps.cfg");
    ASSETS->initWithJsonFile("title_assets.cfg");
    
    CURSOR_CONVERTER->setCamSize(GM_CFG->_camWidth, GM_CFG->_camHeight);
    
    _renderer->createDeviceDependentResources();
}

void TitleEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight, GM_CFG->_framebufferWidth, GM_CFG->_framebufferHeight);
    
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

void TitleEngine::render(double alpha)
{
    _renderer->render();
}

bool TitleEngine::handleInput(Engine* engine)
{
    int menuState = TitleInputManager::getInstance()->getMenuState();
    
    if (TitleInputManager::getInstance()->isLiveMode())
    {
        if (menuState == TIMS_ESCAPE)
        {
            TitleInputManager::getInstance()->setLiveInputMode(false);
            
            _state = _isSteam ? TitleEngineState_SteamOn : TitleEngineState_SteamOff;
        }
        else if (TitleInputManager::getInstance()->isTimeToProcessInput())
        {
            if (_state == TitleEngineState_InputIp)
            {
                _serverIPAddress = StringUtil::format("%s:%d", TitleInputManager::getInstance()->getLiveInput().c_str(), GM_CFG->_serverPort);
                _name.clear();
                _state = TitleEngineState_InputName;
            }
            else if (_state == TitleEngineState_InputName)
            {
                _name = TitleInputManager::getInstance()->getLiveInput();
                if (_name.length() > 0)
                {
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
            }
            
            TitleInputManager::getInstance()->onInputProcessed();
        }
    }
    else if (NG_SERVER)
    {
        if (menuState == TIMS_ESCAPE)
        {
            disconnect();
            return true;
        }
    }
    else
    {
        if (menuState == TIMS_ENTER_STUDIO)
        {
            if (PlatformHelper::getPlatform() != NG_PLATFORM_ANDROID
                && PlatformHelper::getPlatform() != NG_PLATFORM_IOS)
            {
                engine->getStateMachine().changeState(StudioEngine::getInstance());
                return true;
            }
        }
        else if (menuState == TIMS_ACTIVATE_STEAM)
        {
            activateSteam();
        }
        else if (menuState == TIMS_DEACTIVATE_STEAM)
        {
            deactivateSteam();
        }
        else if (menuState == TIMS_START_SERVER)
        {
            if (_isSteam)
            {
                startServer();
            }
            else
            {
                _serverIPAddress.clear();
                _name.clear();
                _state = TitleEngineState_InputName;
                TitleInputManager::getInstance()->setLiveInputMode(true);
            }
        }
        else if (menuState == TIMS_JOIN_LOCAL_SERVER)
        {
            if (!_isSteam)
            {
                _serverIPAddress.clear();
                _state = TitleEngineState_InputIp;
                TitleInputManager::getInstance()->setLiveInputMode(true);
            }
        }
        else if (menuState == TIMS_STEAM_REFRESH_LAN_SERVERS)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES)
            {
                NG_STEAM_GAME_SERVICES->refreshLANServers();
            }
#endif
        }
        else if (menuState == TIMS_STEAM_REFRESH_INTERNET_SERVERS)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES)
            {
                NG_STEAM_GAME_SERVICES->refreshInternetServers();
            }
#endif
        }
        else if (menuState == TIMS_STEAM_JOIN_SERVER_1
                 || menuState == TIMS_STEAM_JOIN_SERVER_2
                 || menuState == TIMS_STEAM_JOIN_SERVER_3
                 || menuState == TIMS_STEAM_JOIN_SERVER_4)
        {
#ifdef NG_STEAM
            if (NG_STEAM_GAME_SERVICES && !NG_STEAM_GAME_SERVICES->isRequestingServers())
            {
                int serverIndex = menuState - TIMS_STEAM_JOIN_SERVER_1; // eh, hacky I know, but whatever
                std::vector<NGSteamGameServer> gameServers = NG_STEAM_GAME_SERVICES->getGameServers();
                if (gameServers.size() > serverIndex)
                {
                    NG_STEAM_GAME_SERVICES->initiateServerConnection(gameServers[serverIndex].getSteamID());
                }
            }
#endif
        }
        else if (menuState == TIMS_ESCAPE)
        {
            engine->setRequestedAction(REQUESTED_ACTION_EXIT);
            return true;
        }
    }
    
    return false;
}

void TitleEngine::activateSteam()
{
    _state = TitleEngineState_SteamOff;
    
#ifdef NG_STEAM
    if (!NGSteamGameServices::getInstance())
    {
        NGSteamGameServices::create(GM_CFG->_steamGameDir.c_str());
    }
    
    _isSteam = NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS;
    _state = _isSteam ? TitleEngineState_SteamOn : TitleEngineState_SteamOff;
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
    _state = TitleEngineState_SteamOff;
#endif
}

void TitleEngine::startServer()
{
    disconnect();
    
    _state = TitleEngineState_ServerStarting;
    
    Server::create(_isSteam);
    
    assert(NG_SERVER);
}

void TitleEngine::joinServer(Engine* engine)
{
    if (_isSteam)
    {
#ifdef NG_STEAM
        NetworkManagerClient::create(new NGSteamClientHelper(_serverSteamID, GameEngine::sGetPlayerAddressHashForIndexOnClient, NG_CLIENT_CALLBACKS), GAME_ENGINE_CALLBACKS, INPUT_MANAGER_CALLBACKS);
#endif
    }
    else
    {
        NetworkManagerClient::create(new SocketClientHelper(_serverIPAddress, _name, GM_CFG->_clientPort, NG_CLIENT_CALLBACKS), GAME_ENGINE_CALLBACKS, INPUT_MANAGER_CALLBACKS);
    }
    
    assert(NG_CLIENT);
    
    engine->getStateMachine().changeState(GameEngine::getInstance());
}

void TitleEngine::disconnect()
{
    if (NG_CLIENT)
    {
        NetworkManagerClient::destroy();
    }
    
    if (Server::getInstance())
    {
        Server::destroy();
    }
    
    _state = _isSteam ? TitleEngineState_SteamOn : TitleEngineState_SteamOff;
}