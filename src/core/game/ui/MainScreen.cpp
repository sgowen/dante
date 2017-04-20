//
//  MainScreen.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

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

#include "client.h"

#define FRAME_RATE 0.01666666666667f // 60 frames per second

MainScreen::MainScreen() : IScreen(),
m_config(new JsonFile("dante.cfg")),
m_renderer(new MainRenderer(MAX_BATCH_SIZE)),
m_touchPointDown(new Vector2D()),
m_touchPointDown2(new Vector2D()),
m_fFrameStateTime(0),
m_iRequestedAction(REQUESTED_ACTION_UPDATE),
m_thread1(nullptr),
m_thread2(nullptr),
m_thread3(nullptr),
m_samus(new PhysicalEntity(3, 3, 1.173913043478261f, 1.5f)),
m_iNetworkAction(0)
{
    m_config->load();
    
    m_serverAddress = m_config->findValue("server_ip");
    
    m_iNetworkAction = 0;
}

MainScreen::~MainScreen()
{
    delete m_config;
    delete m_renderer;
    delete m_touchPointDown;
    delete m_touchPointDown2;
    
    if (m_thread1)
    {
        m_thread1->detach();
        delete m_thread1;
        m_thread1 = nullptr;
    }
    
    if (m_thread2)
    {
        m_thread2->detach();
        delete m_thread2;
        m_thread2 = nullptr;
    }
    
    if (m_thread3)
    {
        m_thread3->detach();
        delete m_thread3;
        m_thread3 = nullptr;
    }
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
    m_iNetworkAction = 0;
}

void MainScreen::onPause()
{
    if (m_iNetworkAction == 1)
    {
        shutDownClient();
    }
    
    m_iNetworkAction = 0;
    
    NG_AUDIO_ENGINE->pause();
}

void MainScreen::update(float deltaTime)
{
    m_fFrameStateTime += deltaTime;
    
    if (m_fFrameStateTime >= FRAME_RATE)
    {
        SCREEN_INPUT_MANAGER->process();
        KEYBOARD_INPUT_MANAGER->process();
        GAME_PAD_INPUT_MANAGER->process();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            tempUpdate(FRAME_RATE);
        }
        
        NG_AUDIO_ENGINE->update();
    }
}

#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <stdlib.h>

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    
    return tokens;
}

std::string delim = ",";

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
    
    char * latestMessage;
    if ((latestMessage = getLatestMessage()))
    {
        std::stringstream ss;
        ss << latestMessage;
        std::string input = ss.str();
        
        std::vector<std::string> coords = split(input, delim);
        if (coords.size() >= 3)
        {
            x1 = atoi(coords.at(1).c_str()) / 100.0f;
            y1 = atoi(coords.at(2).c_str()) / 100.0f;
        }
        
        if (coords.size() >= 6)
        {
            x2 = atoi(coords.at(4).c_str()) / 100.0f;
            y2 = atoi(coords.at(5).c_str()) / 100.0f;
        }
        
        if (coords.size() >= 9)
        {
            x3 = atoi(coords.at(7).c_str()) / 100.0f;
            y3 = atoi(coords.at(8).c_str()) / 100.0f;
        }
        
        if (coords.size() >= 12)
        {
            x4 = atoi(coords.at(10).c_str()) / 100.0f;
            y4 = atoi(coords.at(11).c_str()) / 100.0f;
        }
    }
    
    m_renderer->tempDraw(m_samus->getStateTime(), x1, y1, x2, y2, x3, y3, x4, y4);
    
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

void MainScreen::tempUpdate(float deltaTime)
{
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getType())
        {
            case KeyboardEventType_D:
            case KeyboardEventType_ARROW_KEY_RIGHT:
                m_samus->getPosition().add(0.1f, 0);
                continue;
            case KeyboardEventType_A:
            case KeyboardEventType_ARROW_KEY_LEFT:
                m_samus->getPosition().sub(0.1f, 0);
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
                m_samus->getPosition().add(0.1f, 0);
                continue;
            case GamePadEventType_D_PAD_LEFT:
                m_samus->getPosition().sub(0.1f, 0);
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
                    m_samus->getPosition().add(0.1f, 0);
                }
                else
                {
                    m_samus->getPosition().sub(0.1f, 0);
                }
                continue;
            case ScreenEventType_UP:
                break;
        }
    }
    
    m_samus->update(FRAME_RATE);
    
    m_samus->getPosition().setX(clamp(m_samus->getPosition().getX(), CAM_WIDTH, 0));
    m_samus->getPosition().setY(clamp(m_samus->getPosition().getY(), CAM_HEIGHT, 0));
    
    updateCoords(m_samus->getPosition().getX(), m_samus->getPosition().getY());
    
    if (m_iNetworkAction == 0)
    {
        m_iNetworkAction = 1;
        
        if (m_thread1) { m_thread1->detach(); delete m_thread1; m_thread1 = nullptr; }
        m_thread1 = new std::thread(startClientUDP);
    }
    else if (m_iNetworkAction == 1)
    {
        int clientStatus = getClientStatus();
        if (clientStatus == 1)
        {
            if (m_thread2) { m_thread2->detach(); delete m_thread2; m_thread2 = nullptr; }
            m_thread2 = new std::thread(sendCoords);
        }
        else if (clientStatus == 3)
        {
            if (m_thread3) { m_thread3->detach(); delete m_thread3; m_thread3 = nullptr; }
            m_thread3 = new std::thread(readFromServer);
        }
    }
}

RTTI_IMPL(MainScreen, IScreen);
