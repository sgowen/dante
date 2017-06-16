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
#include "SocketAddressFactory.h"
#include "SocketUtil.h"
#include "InputManager.h"
#include "Timing.h"
#include "World.h"
#include "NGSteamGameServices.h"
#include "InstanceManager.h"
#include "InputState.h"
#include "FWInstanceManager.h"
#include "EntityManager.h"

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
m_touchPointDown(new Vector2()),
m_touchPointDown2(new Vector2()),
m_fStateTime(0),
m_fFrameStateTime(0),
m_isConnected(false)
{
    m_config->load();
    
//    if (NG_STEAM_GAME_SERVICES->init() < 0)
//    {
//        m_iRequestedAction = REQUESTED_ACTION_EXIT;
//        return;
//    }
    
    FWInstanceManager::getClientEntityManager()->init(MainEngine::staticRemoveEntity);
    
    InstanceManager::getClientEntityRegistry()->init(MainEngine::staticAddEntity);
    InstanceManager::getClientEntityRegistry()->registerCreationFunction(NETWORK_TYPE_Robot, Robot::staticCreateClient);
    InstanceManager::getClientEntityRegistry()->registerCreationFunction(NETWORK_TYPE_Projectile, Projectile::staticCreateClient);
    InstanceManager::getClientEntityRegistry()->registerCreationFunction(NETWORK_TYPE_SpacePirate, SpacePirate::staticCreateClient);
    
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
    delete m_touchPointDown;
    delete m_touchPointDown2;
    
    if (Server::getInstance())
    {
        Server::destroy();
    }
    
    NG_STEAM_GAME_SERVICES->deinit();
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
        
        NetworkManagerClient::getInstance()->processIncomingPackets();
        
        InputManager::getInstance()->update();
        
        NG_AUDIO_ENGINE->update();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            InstanceManager::getClientWorld()->update();
        }
        
        NetworkManagerClient::getInstance()->sendOutgoingPackets();
        
        if (!m_isConnected)
        {
            InputState* inputState = InputManager::getInstance()->getInputState();
            if (inputState->isStartingServer())
            {
                startServer();
            }
            else if (inputState->isJoiningServer())
            {
                joinServer();
            }
        }
    }
    
    if (Server::getInstance())
    {
        Server::getInstance()->update(deltaTime);
    }
}

void MainEngine::render()
{
    m_renderer->beginFrame();
    
    m_renderer->tempDraw(m_isConnected ? 5 : Server::getInstance() ? 1 : 0);
    
    m_renderer->renderToScreen();
    
    m_renderer->endFrame();
}

void MainEngine::startServer()
{
    if (!Server::getInstance())
    {
        Server::create();
        
        if (!Server::getInstance()->isInitialized())
        {
            Server::destroy();
        }
    }
}

void MainEngine::joinServer()
{
    m_isConnected = NetworkManagerClient::getInstance()->init(InstanceManager::getClientEntityRegistry(), "localhost:9999", "Noctis Games", FRAME_RATE, InputManager::staticRemoveProcessedMoves, InputManager::staticGetMoveList);
    
    if (m_isConnected)
    {
        InputManager::getInstance()->onConnected();
    }
}

RTTI_IMPL(MainEngine, IEngine);
