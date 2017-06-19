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

void MainEngine::staticAddEntity(Entity* inEntity)
{
    InstanceManager::getClientWorld()->addEntity(inEntity);
}

void MainEngine::staticRemoveEntity(Entity* inEntity)
{
    InstanceManager::getClientWorld()->removeEntity(inEntity);
}

MainEngine::MainEngine() : IEngine(),
m_config(new JsonFile("dante.cfg")),
m_renderer(new MainRenderer(MAX_BATCH_SIZE)),
m_fStateTime(0),
m_fFrameStateTime(0),
m_iEngineState(MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF),
m_isSteam(false)
{
    m_config->load();
    
    FWInstanceManager::getClientEntityManager()->init(MainEngine::staticRemoveEntity);
    
    FWInstanceManager::getClientEntityRegistry()->init(MainEngine::staticAddEntity);
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
    
    leaveServer();
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
        
        if (NG_CLIENT)
        {
            NG_CLIENT->processIncomingPackets();
        }
        
        InputManager::getInstance()->update();
        
        NG_AUDIO_ENGINE->update();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            InstanceManager::getClientWorld()->update();
        }
        
        if (NG_CLIENT)
        {
            NG_CLIENT->sendOutgoingPackets();
            
            if (NG_CLIENT->getState() == NCS_Disconnected)
            {
                leaveServer();
            }
        }
        
        if (NG_STEAM_GAME_SERVICES)
        {
            if (NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS)
            {
                if (NG_STEAM_GAME_SERVICES->isRequestingToJoinServer())
                {
                    leaveServer();
                    m_serverSteamID = NG_STEAM_GAME_SERVICES->getServerToJoinSteamID();
                    joinServer();
                }
            }
            else
            {
                leaveServer();
                deactivateSteam();
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
            leaveServer();
        }
    }
    else if (NG_SERVER)
    {
        if (InputManager::getInstance()->isLiveMode())
        {
            if (inputState->getMenuState() == MENU_STATE_ESCAPE)
            {
                InputManager::getInstance()->setLiveMode(false);
                InputManager::getInstance()->resetLiveInput();
                leaveServer();
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
                m_iEngineState = MAIN_ENGINE_STATE_MAIN_MENU_ENTERING_USERNAME;
                
                if (NG_SERVER->isConnected())
                {
                    if (m_isSteam)
                    {
                        m_serverSteamID = static_cast<NGSteamAddress*>(NG_SERVER->getServerAddress())->getSteamID();
                        joinServer();
                    }
                    else
                    {
                        InputManager::getInstance()->setLiveMode(true);
                    }
                }
            }
            else if (inputState->getMenuState() == MENU_STATE_ESCAPE)
            {
                leaveServer();
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
                    m_serverSteamID = gameServers[serverIndex].getSteamID();
                    m_iEngineState = MAIN_ENGINE_STEAM_JOINING_SERVER;
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
        NGSteamGameServices::create("projectdante");
    }
    
    m_isSteam = NG_STEAM_GAME_SERVICES->getStatus() == STEAM_INIT_SUCCESS;
    m_iEngineState = m_isSteam ? MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON : MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
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
    
    if (m_isSteam)
    {
        NetworkManagerClient::create(new NGSteamClientHelper(m_serverSteamID, NetworkManagerClient::staticProcessPacket, NetworkManagerClient::staticHandleNoResponse, NetworkManagerClient::staticHandleConnectionReset), FRAME_RATE, InputManager::staticRemoveProcessedMoves, InputManager::staticGetMoveList);
        
        NG_STEAM_GAME_SERVICES->onServerJoined();
    }
    else
    {
        // This allows us to run both a debug and a release socket-based client on the same machine
#if defined(DEBUG) || defined(_DEBUG)
        uint16_t port = 1339;
#else
        uint16_t port = 1337;
#endif
        
        NetworkManagerClient::create(new SocketClientHelper(m_serverIPAddress, m_name, port, NetworkManagerClient::staticProcessPacket, NetworkManagerClient::staticHandleNoResponse, NetworkManagerClient::staticHandleConnectionReset), FRAME_RATE, InputManager::staticRemoveProcessedMoves, InputManager::staticGetMoveList);
        
        m_serverIPAddress.clear();
        m_name.clear();
    }
    
    InputManager::getInstance()->setConnected(NG_CLIENT);
}

void MainEngine::leaveServer()
{
    if (NG_CLIENT)
    {
        NetworkManagerClient::destroy();
        
        FWInstanceManager::getClientEntityManager()->reset();
    }
    
    if (Server::getInstance())
    {
        Server::destroy();
    }
    
    m_iEngineState = m_isSteam ? MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON : MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF;
    
    InputManager::getInstance()->setConnected(false);
}

RTTI_IMPL(MainEngine, IEngine);
