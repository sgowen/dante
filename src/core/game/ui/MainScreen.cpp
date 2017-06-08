//
//  MainScreen.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MainScreen.h"

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

MainScreen::MainScreen() : IScreen(),
m_config(new JsonFile("dante.cfg")),
m_renderer(new MainRenderer(MAX_BATCH_SIZE)),
m_touchPointDown(new Vector2()),
m_touchPointDown2(new Vector2()),
m_fStateTime(0),
m_fFrameStateTime(0),
m_iRequestedAction(REQUESTED_ACTION_UPDATE)
{
    m_config->load();
    
    std::string serverIPAddress = m_config->findValue("server_ip");
    if (serverIPAddress.length() == 0)
    {
        //serverIPAddress = std::string("208.97.168.138:9999");
        serverIPAddress = std::string("localhost:9999");
    }
    
    std::string userID = m_config->findValue("user_id");
    if (userID.length() == 0)
    {
        userID = std::string("Noctis Games");
    }
    
    SOCKET_UTIL->init();
    
    NetworkManagerClient::getInstance()->init(serverIPAddress, userID, FRAME_RATE, World::staticRemoveEntity);
    
    EntityRegistry::getInstance()->init(World::staticAddEntity);
    
    EntityRegistry::getInstance()->registerCreationFunction(NETWORK_TYPE_Robot, Robot::create);
    EntityRegistry::getInstance()->registerCreationFunction(NETWORK_TYPE_Projectile, Projectile::create);
    EntityRegistry::getInstance()->registerCreationFunction(NETWORK_TYPE_SpacePirate, SpacePirate::create);
    
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_ROBOT_JUMP, SOUND_ROBOT_JUMP, 4);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_EXPLOSION, SOUND_EXPLOSION, 8);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_DEATH, SOUND_DEATH, 2);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_FIRE_ROCKET, SOUND_FIRE_ROCKET, 8);
    NG_AUDIO_ENGINE->loadSound(SOUND_ID_ACTIVATE_SPRINT, SOUND_ACTIVATE_SPRINT, 4);
    
    NG_AUDIO_ENGINE->loadMusic(MUSIC_DEMO);
}

MainScreen::~MainScreen()
{
    delete m_config;
    delete m_renderer;
    delete m_touchPointDown;
    delete m_touchPointDown2;
}

void MainScreen::createDeviceDependentResources()
{
	m_renderer->createDeviceDependentResources();
}

void MainScreen::createWindowSizeDependentResources(int renderWidth, int renderHeight, int touchScreenWidth, int touchScreenHeight)
{
	m_renderer->createWindowSizeDependentResources(renderWidth, renderHeight, NUM_FRAMEBUFFERS);

	TOUCH_CONVERTER->setTouchScreenSize(touchScreenWidth, touchScreenHeight);
	TOUCH_CONVERTER->setCamSize(CAM_WIDTH, CAM_HEIGHT);
}

void MainScreen::releaseDeviceDependentResources()
{
	m_renderer->releaseDeviceDependentResources();
}

void MainScreen::onResume()
{
    NG_AUDIO_ENGINE->resume();
}

void MainScreen::onPause()
{
    NG_AUDIO_ENGINE->pause();
}

void MainScreen::update(float deltaTime)
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
            
            World::getInstance()->update();
        }
        
        NetworkManagerClient::getInstance()->sendOutgoingPackets();
    }
}

void MainScreen::render()
{
    m_renderer->beginFrame();
    
    m_renderer->tempDraw();
    
    m_renderer->renderToScreen();
    
    m_renderer->endFrame();
}

int MainScreen::getRequestedAction()
{
	return m_iRequestedAction;
}

void MainScreen::clearRequestedAction()
{
    m_iRequestedAction = REQUESTED_ACTION_UPDATE;
}

RTTI_IMPL(MainScreen, IScreen);
