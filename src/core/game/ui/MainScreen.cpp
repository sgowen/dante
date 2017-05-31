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
#include "PhysicalEntity.h"

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

#include "Client.h"
#include "NetworkManagerClient.h"
#include "InputManager.h"
#include "Timing.h"

#define FRAME_RATE 0.01666666666667f // 60 frames per second

MainScreen::MainScreen() : IScreen(),
m_config(new JsonFile("dante.cfg")),
m_renderer(new MainRenderer(MAX_BATCH_SIZE)),
m_touchPointDown(new Vector2()),
m_touchPointDown2(new Vector2()),
m_fStateTime(0),
m_fFrameStateTime(0),
m_iRequestedAction(REQUESTED_ACTION_UPDATE),
m_avatar(new PhysicalEntity(3, 3, 1.173913043478261f, 1.5f))
{
    m_config->load();
    
    std::string serverAddress = m_config->findValue("server_ip");
    if (serverAddress.length() == 0)
    {
        //serverAddress = std::string("208.97.168.138:9999");
        serverAddress = std::string("localhost:9999");
    }
    
    std::string userId = m_config->findValue("user_id");
    if (userId.length() == 0)
    {
        userId = std::string("Noctis Games");
    }
    
    InputManager::StaticInit();
    
    Client::getInstance()->init(serverAddress, userId);
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
    // TODO
}

void MainScreen::onPause()
{
    NG_AUDIO_ENGINE->pause();
}

void MainScreen::update(float deltaTime)
{
    m_fStateTime += deltaTime;
    m_fFrameStateTime += deltaTime;
    
    Timing::sInstance.updateManual(m_fStateTime, deltaTime);
    
    if (m_fFrameStateTime >= FRAME_RATE)
    {
        NetworkManagerClient::sInstance->ProcessIncomingPackets();
        
        tempUpdateInput();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            tempUpdate(FRAME_RATE);
        }
        
        NG_AUDIO_ENGINE->update();
        
        NetworkManagerClient::getInstance()->SendOutgoingPackets();
    }
}

void MainScreen::render()
{
    m_renderer->beginFrame();
    
    m_renderer->tempDraw(m_avatar->getStateTime());
    
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

void MainScreen::tempUpdateInput()
{
    SCREEN_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getType())
        {
            case KeyboardEventType_W:
                InputManager::sInstance->HandleInput((*i)->isUp() ? EIA_Released : EIA_Pressed, 'W');
                continue;
            case KeyboardEventType_A:
                InputManager::sInstance->HandleInput((*i)->isUp() ? EIA_Released : EIA_Pressed, 'A');
                continue;
            case KeyboardEventType_S:
                InputManager::sInstance->HandleInput((*i)->isUp() ? EIA_Released : EIA_Pressed, 'S');
                continue;
            case KeyboardEventType_D:
                InputManager::sInstance->HandleInput((*i)->isUp() ? EIA_Released : EIA_Pressed, 'D');
                continue;
            default:
                continue;
        }
    }
    
    for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getType())
        {
            case GamePadEventType_D_PAD_UP:
                InputManager::sInstance->HandleInput((*i)->isButtonPressed() ? EIA_Pressed : EIA_Released, 'W');
                continue;
            case GamePadEventType_D_PAD_LEFT:
                InputManager::sInstance->HandleInput((*i)->isButtonPressed() ? EIA_Pressed : EIA_Released, 'A');
                continue;
            case GamePadEventType_D_PAD_DOWN:
                InputManager::sInstance->HandleInput((*i)->isButtonPressed() ? EIA_Pressed : EIA_Released, 'S');
                continue;
            case GamePadEventType_D_PAD_RIGHT:
                InputManager::sInstance->HandleInput((*i)->isButtonPressed() ? EIA_Pressed : EIA_Released, 'D');
                continue;
            default:
                continue;
        }
    }
    
    InputManager::sInstance->Update();
}

void MainScreen::tempUpdate(float deltaTime)
{
    Client::getInstance()->DoFrame();
    
    m_avatar->update(FRAME_RATE);
}

RTTI_IMPL(MainScreen, IScreen);
