//
//  MainEngine.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MainEngine.h"

#include "Server.h"
#include "JsonFile.h"
#include "MainRenderer.h"
#include "Vector2.h"
#include "Entity.h"

#include "GameConstants.h"
#include "ScreenInputManager.h"
#include "ScreenEvent.h"
#include "TouchConverter.h"
#include "KeyboardInputManager.h"
#include "KeyboardEvent.h"
#include "GamePadInputManager.h"
#include "GamePadEvent.h"
#include "NGAudioEngine.h"
#include "StringUtil.h"
#include "MathUtil.h"
#include "Robot.h"
#include "Projectile.h"
#include "SpacePirate.h"
#include "EntityRegistry.h"
#include "NetworkManagerClient.h"
#include "NetworkManagerServer.h"
#include "SocketAddressFactory.h"
#include "SocketUtil.h"
#include "InputManager.h"
#include "Timing.h"
#include "World.h"
#include "InstanceManager.h"
#include "InputState.h"
#include "FWInstanceManager.h"
#include "EntityManager.h"
#include "SocketClientHelper.h"
#include "NGSteamClientHelper.h"
#include "IMachineAddress.h"
#include "NGSteamAddress.h"
#include "NGSteamGameServices.h"
#include "MainEngineState.h"

MainEngine::MainEngine() : IEngine(),
m_config(new JsonFile("dante.cfg")),
m_renderer(new MainRenderer(MAX_BATCH_SIZE)),
m_fStateTime(0),
m_fFrameStateTime(0),
m_iEngineState(MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF),
m_isSteam(false)
{
    activateSteam();
    
    m_config->load();
    
    FWInstanceManager::getClientEntityRegistry()->registerCreationFunction(NETWORK_TYPE_Robot, Robot::staticCreateClient);
    FWInstanceManager::getClientEntityRegistry()->registerCreationFunction(NETWORK_TYPE_Projectile, Projectile::staticCreateClient);
    FWInstanceManager::getClientEntityRegistry()->registerCreationFunction(NETWORK_TYPE_SpacePirate, SpacePirate::staticCreateClient);
    
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_ROBOT_JUMP, SOUND_ROBOT_JUMP, 4);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_EXPLOSION, SOUND_EXPLOSION, 8);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_DEATH, SOUND_DEATH, 2);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_FIRE_ROCKET, SOUND_FIRE_ROCKET, 8);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_ACTIVATE_SPRINT, SOUND_ACTIVATE_SPRINT, 4);
    
    NG_AUDIO_ENGINE->loadMusic(MUSIC_DEMO);
}

MainEngine::~MainEngine()
{
    delete m_config;
    delete m_renderer;
    
    disconnect();
	deactivateSteam();
}

void MainEngine::createDeviceDependentResources()
{
	m_renderer->createDeviceDependentResources();
}

void MainEngine::createWindowSizeDependentResources(int renderWidth, int renderHeight, int touchScreenWidth, int touchScreenHeight)
{
	m_renderer->createWindowSizeDependentResources(renderWidth, renderHeight, NUM_FRAMEBUFFERS);

	TOUCH_CONVERTER->setTouchScreenSize(touchScreenWidth, touchScreenHeight);
	TOUCH_CONVERTER->setCamSize(CAM_WIDTH, CAM_HEIGHT);
}

void MainEngine::releaseDeviceDependentResources()
{
	m_renderer->releaseDeviceDependentResources();
}

void MainEngine::onResume()
{
    NG_AUDIO_ENGINE->resume();
}

void MainEngine::onPause()
{
    NG_AUDIO_ENGINE->pause();
}

void MainEngine::update(float deltaTime)
{
    m_fStateTime += deltaTime;
    m_fFrameStateTime += deltaTime;
    
    if (m_fFrameStateTime >= FRAME_RATE)
    {
        Timing::getInstance()->updateManual(m_fStateTime, FRAME_RATE);
        
        handleSteamGameServices();
        
        if (NG_CLIENT)
        {
            NG_CLIENT->processIncomingPackets();
        }
        
        InputManager::getInstance()->update();
        
        NG_AUDIO_ENGINE->update();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            if (InstanceManager::getClientWorld())
            {
                InstanceManager::getClientWorld()->update();
            }
        }
        
        if (NG_CLIENT)
        {
            NG_CLIENT->sendOutgoingPackets();
            
            if (NG_CLIENT->getState() == NCS_Disconnected)
            {
                disconnect();
            }
        }
        
        handleNonMoveInput();
    }
    
    if (Server::getInstance())
    {
        Server::getInstance()->update(deltaTime);
    }
}

void MainEngine::render()
{
    m_renderer->beginFrame();
    
    m_renderer->renderWorld(m_iEngineState);
    
    m_renderer->renderToScreen();
    
    m_renderer->endFrame();
}

void MainEngine::handleNonMoveInput()
{
    InputState* inputState = InputManager::getInstance()->getInputState();
    
    if (NG_CLIENT)
    {
        if (inputState->getMenuState() == MENU_STATE_ESCAPE)
        {
            disconnect();
        }
    }
    else if (NG_SERVER)
    {
        if (m_isSteam)
        {
            if (NG_SERVER->isConnected())
            {
                m_serverSteamID = static_cast<NGSteamAddress*>(NG_SERVER->getServerAddress())->getSteamID();
                joinServer();
            }
        }
        else if (InputManager::getInstance()->isLiveMode())
        {
            if (inputState->getMenuState() == MENU_STATE_ESCAPE)
            {
                InputManager::getInstance()->setLiveMode(false);
                InputManager::getInstance()->resetLiveInput();
                disconnect();
            }
            else if (InputManager::getInstance()->isTimeToProcessInput())
            {
                if (m_serverIPAddress.length() == 0)
                {
                    m_serverIPAddress = std::string("localhost:9999");
                    m_name = InputManager::getInstance()->getLiveInput();
                    InputManager::getInstance()->setLiveMode(false);
                    joinServer();
                }
                
                InputManager::getInstance()->resetLiveInput();
            }
        }
        else
        {
            if (inputState->getMenuState() == MENU_STATE_JOIN_LOCAL_SERVER)
            {
                if (NG_SERVER->isConnected())
                {
                    m_iEngineState = MAIN_ENGINE_STATE_MAIN_MENU_ENTERING_USERNAME;
                    InputManager::getInstance()->setLiveMode(true);
                }
            }
            else if (inputState->getMenuState() == MENU_STATE_ESCAPE)
            {
                disconnect();
            }
        }
    }
    else if (InputManager::getInstance()->isLiveMode())
    {
        if (inputState->getMenuState() == MENU_STATE_ESCAPE)
        {
            InputManager::getInstance()->setLiveMode(false);
            InputManager::getInstance()->resetLiveInput();
            m_iEngineState = m_isSteam ? MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON : MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
        }
        else if (InputManager::getInstance()->isTimeToProcessInput())
        {
            if (m_iEngineState == MAIN_ENGINE_STATE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP)
            {
                m_serverIPAddress = InputManager::getInstance()->getLiveInput();
                m_name.clear();
                m_iEngineState = MAIN_ENGINE_STATE_MAIN_MENU_ENTERING_USERNAME;
            }
            else
            {
                m_name = InputManager::getInstance()->getLiveInput();
                InputManager::getInstance()->setLiveMode(false);
                joinServer();
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
            startServer();
        }
        else if (inputState->getMenuState() == MENU_STATE_JOIN_LOCAL_SERVER)
        {
            if (!m_isSteam)
            {
                m_iEngineState = MAIN_ENGINE_STATE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP;
                m_serverIPAddress.clear();
                InputManager::getInstance()->setLiveMode(true);
            }
        }
        else if (inputState->getMenuState() == MENU_STATE_STEAM_REFRESH_LAN_SERVERS)
        {
            NG_STEAM_GAME_SERVICES->refreshLANServers();
        }
        else if (inputState->getMenuState() == MENU_STATE_STEAM_REFRESH_INTERNET_SERVERS)
        {
            NG_STEAM_GAME_SERVICES->refreshInternetServers();
        }
        else if (inputState->getMenuState() == MENU_STATE_STEAM_JOIN_SERVER_1
                 || inputState->getMenuState() == MENU_STATE_STEAM_JOIN_SERVER_2
                 || inputState->getMenuState() == MENU_STATE_STEAM_JOIN_SERVER_3
                 || inputState->getMenuState() == MENU_STATE_STEAM_JOIN_SERVER_4)
        {
            if (!NG_STEAM_GAME_SERVICES->isRequestingServers())
            {
                int serverIndex = inputState->getMenuState() - 7; // eh, hacky I know, but whatever
                std::vector<NGSteamGameServer> gameServers = NG_STEAM_GAME_SERVICES->getGameServers();
                if (gameServers.size() > serverIndex)
                {
                    NG_STEAM_GAME_SERVICES->initiateServerConnection(gameServers[serverIndex].getSteamID());
                }
            }
        }
        else if (inputState->getMenuState() == MENU_STATE_ESCAPE)
        {
            m_iRequestedAction = REQUESTED_ACTION_EXIT;
        }
    }
}

void MainEngine::activateSteam()
{
    if (!NGSteamGameServices::getInstance())
    {
        NGSteamGameServices::create(STEAM_GAME_DIR);
    }
    
    m_isSteam = NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS;
    m_iEngineState = m_isSteam ? MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON : MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
}

void MainEngine::handleSteamGameServices()
{
    if (NG_STEAM_GAME_SERVICES)
    {
        NG_STEAM_GAME_SERVICES->update(NG_SERVER ? true : false);
        
        if (NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS)
        {
            if (NG_STEAM_GAME_SERVICES->isRequestingToJoinServer())
            {
                disconnect();
                m_serverSteamID = NG_STEAM_GAME_SERVICES->getServerToJoinSteamID();
                joinServer();
            }
        }
        else
        {
            disconnect();
            deactivateSteam();
        }
    }
}

void MainEngine::deactivateSteam()
{
    if (NGSteamGameServices::getInstance())
    {
        NGSteamGameServices::destroy();
    }
    
    m_isSteam = false;
    m_iEngineState = MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
}

void MainEngine::startServer()
{
    m_iEngineState = MAIN_ENGINE_STATE_MAIN_MENU_STARTING_SERVER;
    
    if (!Server::getInstance())
    {
        Server::create(m_isSteam);
        
        if (!NG_SERVER)
        {
            Server::destroy();
        }
    }
}

void MainEngine::joinServer()
{
    m_iEngineState = MAIN_ENGINE_STEAM_JOINING_SERVER;
    
    FWInstanceManager::createClientEntityManager(InstanceManager::staticHandleEntityCreatedOnClient, InstanceManager::staticHandleEntityDeletedOnClient);
    
    InstanceManager::createClientWorld();
    
    IClientHelper* clientHelper = nullptr;
    if (m_isSteam)
    {
        clientHelper = new NGSteamClientHelper(m_serverSteamID, InstanceManager::staticGetPlayerAddressHashOnClient, NG_CLIENT_CALLBACKS);
    }
    else
    {
        clientHelper = new SocketClientHelper(m_serverIPAddress, m_name, CLIENT_PORT, NG_CLIENT_CALLBACKS);
    }
    
    NetworkManagerClient::create(clientHelper, FRAME_RATE, INPUT_MANAGER_CALLBACKS);
    
    InputManager::getInstance()->setConnected(NG_CLIENT);
}

void MainEngine::disconnect()
{
    if (NG_CLIENT)
    {
        NetworkManagerClient::destroy();
        
        InstanceManager::destroyClientWorld();
        
        FWInstanceManager::destroyClientEntityManager();
    }
    
    if (Server::getInstance())
    {
        Server::destroy();
    }
    
    m_iEngineState = m_isSteam ? MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON : MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
    
    InputManager::getInstance()->setConnected(false);
}

RTTI_IMPL(MainEngine, IEngine);
