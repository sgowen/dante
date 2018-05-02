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

#include <game/game/Server.h>
#include <framework/util/Constants.h>
#include <framework/input/CursorInputManager.h>
#include <framework/input/CursorEvent.h>
#include <framework/input/CursorConverter.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <game/title/TitleInputManager.h>
#include <framework/studio/World.h>
#include <game/game/GameInputState.h>
#include <framework/entity/EntityManager.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/PlatformHelper.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/Constants.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/Timing.h>
#include <framework/util/FPSUtil.h>
#include <framework/input/CursorConverter.h>
#include <game/game/GameEngine.h>
#include <game/game/GameInputManager.h>
#include <framework/studio/StudioEngine.h>
#include <game/config/GameConfig.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/util/Config.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamGameServices.h>
#endif

IMPL_RTTI(TitleEngine, EngineState);

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

void TitleEngine::enter(Engine* engine)
{
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getCursorWidth(), engine->getCursorHeight());
    
    _state = _isSteam ? TES_SteamOn : TES_SteamOff;
}

void TitleEngine::update(Engine* engine)
{
    handleSteamGameServices(engine);
    
    TitleInputManager::getInstance()->update();
    
    handleInput(engine);
}

void TitleEngine::exit(Engine* engine)
{
    releaseDeviceDependentResources();
    
    TitleInputManager::destroy();
}

void TitleEngine::createDeviceDependentResources()
{
    TitleInputManager::create();
    
    GM_CFG->initWithJsonFile("global.cfg");
    EntityMapper::getInstance()->initWithJsonFile("entities.cfg");
    EntityLayoutMapper::getInstance()->initWithJsonFile("maps.cfg");
    ASSETS->initWithJsonFile("title_assets.cfg");
    
    CURSOR_CONVERTER->setCamSize(FW_CFG->_camWidth, FW_CFG->_camHeight);
    
    _renderer->createDeviceDependentResources();
}

void TitleEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight);
    
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

void TitleEngine::handleInput(Engine* engine)
{
    int menuState = TitleInputManager::getInstance()->getMenuState();
    
    if (TitleInputManager::getInstance()->isLiveMode())
    {
        if (menuState == TIMS_ESCAPE)
        {
            TitleInputManager::getInstance()->setLiveInputMode(false);
            
            _state = _isSteam ? TES_SteamOn : TES_SteamOff;
        }
        else if (TitleInputManager::getInstance()->isTimeToProcessInput())
        {
            bool needsToProcessInput = true;
            if (_state == TES_InputIp)
            {
                _serverIPAddress = StringUtil::format("%s:%d", TitleInputManager::getInstance()->getLiveInput().c_str(), FW_CFG->_serverPort);
                _name.clear();
                _state = TES_InputName;
            }
            else if (_state == TES_InputName)
            {
                _name = TitleInputManager::getInstance()->getLiveInput();
                if (_name.length() > 0)
                {
                    TitleInputManager::getInstance()->setLiveInputMode(false);
                    
                    if (_serverIPAddress.length() == 0)
                    {
                        GameEngine::sHandleHostServer(engine, _name);
                    }
                    else
                    {
                        GameEngine::sHandleJoinServer(engine, _serverIPAddress, _name);
                    }
                    
                    needsToProcessInput = false;
                }
            }
            
            if (needsToProcessInput)
            {
                TitleInputManager::getInstance()->onInputProcessed();
            }
        }
    }
    else
    {
        if (menuState == TIMS_ENTER_STUDIO)
        {
            engine->getStateMachine().changeState(StudioEngine::getInstance());
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
#ifdef NG_STEAM
                GameEngine::sHandleHostSteamServer(engine);
#endif
            }
            else
            {
                _serverIPAddress.clear();
                _name.clear();
                _state = TES_InputName;
                TitleInputManager::getInstance()->setLiveInputMode(true);
            }
        }
        else if (menuState == TIMS_JOIN_LOCAL_SERVER)
        {
            if (!_isSteam)
            {
                _serverIPAddress.clear();
                _state = TES_InputIp;
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
            deactivateSteam();
            engine->setRequestedAction(REQUESTED_ACTION_EXIT);
        }
    }
}

void TitleEngine::activateSteam()
{
    _state = TES_SteamOff;
    
#ifdef NG_STEAM
    if (!NG_STEAM_GAME_SERVICES)
    {
        NGSteamGameServices::create(FW_CFG->_steamGameDir.c_str());
    }
    
    _isSteam = NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS;
    _state = _isSteam ? TES_SteamOn : TES_SteamOff;
#endif
}

void TitleEngine::deactivateSteam()
{
#ifdef NG_STEAM
    if (NG_STEAM_GAME_SERVICES)
    {
        NGSteamGameServices::destroy();
    }
    
    _isSteam = false;
    _state = TES_SteamOff;
#endif
}

void TitleEngine::handleSteamGameServices(Engine* engine)
{
#ifdef NG_STEAM
    if (NG_STEAM_GAME_SERVICES)
    {
        NG_STEAM_GAME_SERVICES->update();
        
        if (NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS)
        {
            if (NG_STEAM_GAME_SERVICES->isRequestingToJoinServer())
            {
                GameEngine::sHandleJoinSteamServer(engine, NG_STEAM_GAME_SERVICES->getServerToJoinSteamID());
            }
        }
        else
        {
            deactivateSteam();
        }
    }
#endif
}

TitleEngine::TitleEngine() : EngineState(),
_renderer(new TitleRenderer()),
_isSteam(false),
_state(TES_SteamOff)
{
    activateSteam();
}

TitleEngine::~TitleEngine()
{
    delete _renderer;
}
