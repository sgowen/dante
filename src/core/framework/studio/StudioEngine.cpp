//
//  StudioEngine.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/studio/StudioEngine.h>

#include <framework/file/portable/JsonFile.h>
#include <framework/studio/StudioRenderer.h>

#include <framework/util/Constants.h>
#include <framework/input/CursorInputManager.h>
#include <framework/input/CursorEvent.h>
#include <framework/input/CursorConverter.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/studio/StudioInputManager.h>
#include <framework/studio/World.h>
#include <framework/entity/EntityManager.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/NGExtension.h>
#include <framework/util/PlatformHelper.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/Constants.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/util/FPSUtil.h>
#include <framework/input/CursorConverter.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/util/macros.h>
#include <framework/util/Config.h>

IMPL_RTTI(StudioEngine, EngineState);

StudioEngine* StudioEngine::s_instance = NULL;

void StudioEngine::create(TestFunc testFunc)
{
    assert(!s_instance);
    
    s_instance = new StudioEngine(testFunc);
}

StudioEngine * StudioEngine::getInstance()
{
    return s_instance;
}

void StudioEngine::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void StudioEngine::enter(Engine* engine)
{
    _engine = engine;
    
    createDeviceDependentResources();
    createWindowSizeDependentResources(engine->getScreenWidth(), engine->getScreenHeight(), engine->getCursorWidth(), engine->getCursorHeight());
}

void StudioEngine::update(Engine* engine)
{
    StudioInputManager::getInstance()->update();
    
    if (StudioInputManager::getInstance()->getMenuState() == SIMS_ESCAPE)
    {
        engine->getStateMachine().revertToPreviousState();
    }
}

void StudioEngine::exit(Engine* engine)
{
    releaseDeviceDependentResources();
}

void StudioEngine::createDeviceDependentResources()
{
    FW_CFG->initWithJsonFile("global.cfg");
    EntityMapper::getInstance()->initWithJsonFile("entities.cfg");
    EntityLayoutMapper::getInstance()->initWithJsonFile("maps.cfg");
    ASSETS->initWithJsonFile("game_assets.cfg");
    
    CURSOR_CONVERTER->setCamSize(FW_CFG->_camWidth, FW_CFG->_camHeight);
    
    _renderer->createDeviceDependentResources();
}

void StudioEngine::createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer->createWindowSizeDependentResources(screenWidth, screenHeight);
    
    CURSOR_CONVERTER->setCursorSize(cursorWidth, cursorHeight);
}

void StudioEngine::releaseDeviceDependentResources()
{
    _renderer->releaseDeviceDependentResources();
}

void StudioEngine::onResume()
{
    // Empty
}

void StudioEngine::onPause()
{
    // Empty
}

void StudioEngine::render(double alpha)
{
    _renderer->render();
}

StudioEngine::StudioEngine(TestFunc testFunc) : EngineState(),
_renderer(new StudioRenderer()),
_world(new World(WorldFlag_Studio)),
_engine(NULL),
_testFunc(testFunc),
_state(StudioEngineState_DisplayGrid | StudioEngineState_DisplayControls | StudioEngineState_LayerAll),
_textInputField(0),
_textInputType(0)
{
    StudioInputManager::create();
    StudioInputManager::getInstance()->setEngine(this);
    
    _renderer->setEngine(this);
    _renderer->setInputManager(StudioInputManager::getInstance());
}

StudioEngine::~StudioEngine()
{
    delete _renderer;
    delete _world;
}
