//
//  MainScreen.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "MainScreen.h"

#include "IDeviceHelper.h"
#include "MainRenderer.h"
#include "Vector2D.h"

#include "GameConstants.h"
#include "ScreenInputManager.h"
#include "KeyboardInputManager.h"
#include "GamePadInputManager.h"
#include "KeyboardEvent.h"
#include "GamePadEvent.h"
#include "TouchConverter.h"
#include "MainRenderer.h"
#include "DeviceHelperFactory.h"
#include "ScreenEvent.h"
#include "NGAudioEngine.h"
#include "SaveData.h"

#define FRAME_RATE 0.01666666666667f // 60 frames per second

MainScreen::MainScreen() : IScreen(),
m_deviceHelper(DEVICE_HELPER_FACTORY->createDeviceHelper()),
m_renderer(new MainRenderer()),
m_touchPointDown(new Vector2D()),
m_touchPointDown2(new Vector2D()),
m_fFrameStateTime(0),
m_iRequestedAction(REQUESTED_ACTION_UPDATE)
{
#if defined __ANDROID__
    NG_SAVE_DATA->config("/data/data/com.noctisgames.dante/files/data.sav");
#else
    NG_SAVE_DATA->config("data.sav");
#endif
}

MainScreen::~MainScreen()
{
    delete m_deviceHelper;
    delete m_renderer;
    delete m_touchPointDown;
    delete m_touchPointDown2;
}

void MainScreen::createDeviceDependentResources()
{
    m_deviceHelper->createDeviceDependentResources(MAX_BATCH_SIZE);
    
    m_renderer->createDeviceDependentResources();
}

void MainScreen::createWindowSizeDependentResources(int renderWidth, int renderHeight, int touchScreenWidth, int touchScreenHeight)
{
    TOUCH_CONVERTER->setTouchScreenSize(touchScreenWidth, touchScreenHeight);
    TOUCH_CONVERTER->setCamSize(CAM_WIDTH, CAM_HEIGHT);
    
    m_deviceHelper->createWindowSizeDependentResources(renderWidth, renderHeight, NUM_FRAMEBUFFERS);
}

void MainScreen::releaseDeviceDependentResources()
{
    m_deviceHelper->releaseDeviceDependentResources();
    
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
    while (m_fFrameStateTime >= FRAME_RATE)
    {
        m_fFrameStateTime -= FRAME_RATE;
        
        internalUpdate();
    }
}

void MainScreen::render()
{
    m_renderer->beginFrame();
    
    // TODO
    
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

void MainScreen::internalUpdate()
{
    SCREEN_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    // TODO
    
    NG_AUDIO_ENGINE->update();
}

RTTI_IMPL(MainScreen, IScreen);
