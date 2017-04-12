//
//  MainScreen.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "MainScreen.h"

#include "MainRenderer.h"
#include "Vector2D.h"

#include "GameConstants.h"
#include "ScreenInputManager.h"
#include "ScreenEvent.h"
#include "TouchConverter.h"
#include "KeyboardInputManager.h"
#include "KeyboardEvent.h"
#include "GamePadInputManager.h"
#include "GamePadEvent.h"
#include "NGAudioEngine.h"

#define FRAME_RATE 0.01666666666667f // 60 frames per second

MainScreen::MainScreen() : IScreen(),
m_renderer(new MainRenderer(MAX_BATCH_SIZE)),
m_touchPointDown(new Vector2D()),
m_touchPointDown2(new Vector2D()),
m_fFrameStateTime(0),
m_iRequestedAction(REQUESTED_ACTION_UPDATE)
{
    // Empty
}

MainScreen::~MainScreen()
{
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
    m_fFrameStateTime += deltaTime;
    
    if (m_fFrameStateTime >= FRAME_RATE)
    {
        SCREEN_INPUT_MANAGER->process();
        KEYBOARD_INPUT_MANAGER->process();
        GAME_PAD_INPUT_MANAGER->process();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            // TODO, update
        }
        
        NG_AUDIO_ENGINE->update();
    }
}

void MainScreen::render()
{
    m_renderer->beginFrame();
    
    // TODO, render stuff
    
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
