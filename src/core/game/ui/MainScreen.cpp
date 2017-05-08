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
#include "Vector2D.h"
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

#define FRAME_RATE 0.01666666666667f // 60 frames per second

MainScreen::MainScreen() : IScreen(),
m_config(new JsonFile("dante.cfg")),
m_renderer(new MainRenderer(MAX_BATCH_SIZE)),
m_touchPointDown(new Vector2D()),
m_touchPointDown2(new Vector2D()),
m_fFrameStateTime(0),
m_iRequestedAction(REQUESTED_ACTION_UPDATE),
m_avatar(new PhysicalEntity(3, 3, 1.173913043478261f, 1.5f))
{
    m_config->load();
    
    std::string serverAddress = m_config->findValue("server_ip");
    if (serverAddress.length() == 0)
    {
        serverAddress = std::string("208.97.168.138:9999");
    }
    
    std::string userId = m_config->findValue("user_id");
    if (userId.length() == 0)
    {
        userId = std::string("accounts@noctisgames.com");
    }
    
//    CLIENT->init(serverAddress, userId);
}

MainScreen::~MainScreen()
{
    delete m_config;
    delete m_renderer;
    delete m_touchPointDown;
    delete m_touchPointDown2;
    
//    CLIENT->deinit();
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
    m_fFrameStateTime += deltaTime;
    
    if (m_fFrameStateTime >= FRAME_RATE)
    {
//        CLIENT->beginFrame();
        
        tempUpdateInput();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            tempUpdate(FRAME_RATE);
        }
        
        NG_AUDIO_ENGINE->update();
        
//        CLIENT->endFrame();
    }
}

void MainScreen::render()
{
    m_renderer->beginFrame();
    
    float x1 = 0;
    float y1 = 0;
    
    float x2 = 0;
    float y2 = 0;
    
    float x3 = 0;
    float y3 = 0;
    
    float x4 = 0;
    float y4 = 0;
    
    m_renderer->tempDraw(m_avatar->getStateTime(), x1, y1, x2, y2, x3, y3, x4, y4);
    
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
            case KeyboardEventType_D:
            case KeyboardEventType_ARROW_KEY_RIGHT:
                m_avatar->getPosition().add(0.1f, 0);
                continue;
            case KeyboardEventType_A:
            case KeyboardEventType_ARROW_KEY_LEFT:
                m_avatar->getPosition().sub(0.1f, 0);
                continue;
            default:
                continue;
        }
    }
    
    for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getType())
        {
            case GamePadEventType_D_PAD_RIGHT:
                m_avatar->getPosition().add(0.1f, 0);
                continue;
            case GamePadEventType_D_PAD_LEFT:
                m_avatar->getPosition().sub(0.1f, 0);
                continue;
            default:
                continue;
        }
    }
    
    for (std::vector<ScreenEvent *>::iterator i = SCREEN_INPUT_MANAGER->getEvents().begin(); i != SCREEN_INPUT_MANAGER->getEvents().end(); ++i)
    {
        Vector2D& touchPoint = TOUCH_CONVERTER->touchToWorld(*(*i));
        
        switch ((*i)->getType())
        {
            case ScreenEventType_DOWN:
            case ScreenEventType_DRAGGED:
                if (touchPoint.getX() > CAM_WIDTH / 2)
                {
                    m_avatar->getPosition().add(0.1f, 0);
                }
                else
                {
                    m_avatar->getPosition().sub(0.1f, 0);
                }
                continue;
            case ScreenEventType_UP:
                break;
        }
    }
}

void MainScreen::tempUpdate(float deltaTime)
{
    m_avatar->update(FRAME_RATE);
    
    m_avatar->getPosition().setX(clamp(m_avatar->getPosition().getX(), CAM_WIDTH, 0));
    m_avatar->getPosition().setY(clamp(m_avatar->getPosition().getY(), CAM_HEIGHT, 0));
}

RTTI_IMPL(MainScreen, IScreen);
