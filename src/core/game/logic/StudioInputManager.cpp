//
//  StudioInputManager.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/StudioInputManager.h"

#include "game/logic/StudioEngine.h"

#include "framework/util/Timing.h"
#include "framework/input/CursorInputManager.h"
#include "framework/input/CursorEvent.h"
#include "framework/input/CursorConverter.h"
#include "framework/input/KeyboardInputManager.h"
#include "framework/input/KeyboardEvent.h"
#include "framework/input/GamePadInputManager.h"
#include "framework/input/GamePadEvent.h"
#include "game/logic/PooledObjectsManager.h"
#include "game/logic/GameConstants.h"
#include "framework/input/KeyboardLookup.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include "framework/util/FrameworkConstants.h"
#include "game/graphics/StudioRenderer.h"
#include "game/logic/World.h"

#include <sstream>

StudioInputManager* StudioInputManager::s_instance = NULL;

void StudioInputManager::create()
{
    assert(!s_instance);
    
    s_instance = new StudioInputManager();
}

StudioInputManager * StudioInputManager::getInstance()
{
    return s_instance;
}

void StudioInputManager::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void StudioInputManager::update(StudioEngine* engine)
{
    CURSOR_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    _inputState = SIS_NONE;
    
    if (_isLiveMode)
    {
        std::stringstream ss;
        
        for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            KeyboardEvent* keyboardEvent = (*i);
            if (isKeySupported(keyboardEvent->getKey()))
            {
                if (keyboardEvent->isDown()
                    && !keyboardEvent->isHeld())
                {
                    if (keyboardEvent->getKey() == NG_KEY_CARRIAGE_RETURN)
                    {
                        _isTimeToProcessInput = true;
                        return;
                    }
                    else if (keyboardEvent->getKey() == NG_KEY_ESCAPE)
                    {
                        return;
                    }
                    else if (keyboardEvent->getKey() == NG_KEY_BACK_SPACE
                             || keyboardEvent->getKey() == NG_KEY_DELETE)
                    {
                        std::string s = ss.str();
                        _liveInput += s;
                        if (_liveInput.end() > _liveInput.begin())
                        {
                            _liveInput.erase(_liveInput.end() - 1, _liveInput.end());
                        }
                        return;
                    }
                    else
                    {
						unsigned short key = keyboardEvent->getKey();
#ifdef _WIN32
						if (key == NG_KEY_PERIOD)
						{
							key = NG_KEY_ASCII_PERIOD;
						}
#endif
                        ss << StringUtil::format("%c", key);
                    }
                }
                else if (keyboardEvent->isUp())
                {
                    if (keyboardEvent->getKey() == NG_KEY_ESCAPE)
                    {
                        _inputState = SIS_ESCAPE;
                        return;
                    }
                }
            }
        }
        
        std::string s = ss.str();
        _liveInput += s;
        if (_liveInput.length() > 16)
        {
            int sub = static_cast<int>(_liveInput.length()) - 16;
            _liveInput.erase(_liveInput.end() - sub, _liveInput.end());
        }
    }
    else
    {
        _scrollValue += CURSOR_INPUT_MANAGER->getScrollWheelValue();
        CURSOR_INPUT_MANAGER->resetScrollValue();
        _scrollValue = clamp(_scrollValue, 8, 1);
        CURSOR_CONVERTER->setCamSize(CAM_WIDTH * _scrollValue, CAM_HEIGHT * _scrollValue);
        
        for (std::vector<CursorEvent *>::iterator i = CURSOR_INPUT_MANAGER->getEvents().begin(); i != CURSOR_INPUT_MANAGER->getEvents().end(); ++i)
        {
            CursorEvent& e = *(*i);
            Vector2& c = CURSOR_CONVERTER->convert(e);
            switch (e.getType())
            {
                case CursorEventType_DOWN:
                    _downCursor.set(c);
                    _dragCursor.set(c);
                    _deltaCursor.set(0, 0);
                    continue;
                case CursorEventType_DRAGGED:
                {
                    Vector2 delta = c;
                    delta -= _dragCursor;
                    _dragCursor.set(c);
                    _deltaCursor.set(delta);
                    
                    float dx = _deltaCursor.getX() * 2;
                    float dy = _deltaCursor.getY() * 2;
                    float x = _cursor.getX();
                    float y = _cursor.getY();
                    x -= dx;
                    y -= dy;
                    
                    _cursor.set(x, y);
                    _deltaCursor.set(0, 0);
                }
                    continue;
                case CursorEventType_UP:
                    _upCursor.set(c);
                    _deltaCursor.set(0, 0);
                    break;
                default:
                    break;
            }
        }
        
        for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            KeyboardEvent& e = *(*i);
            switch (e.getKey())
            {
                case NG_KEY_CMD:
                case NG_KEY_CTRL:
                    _isControl = !e.isUp();
                    continue;
                case NG_KEY_ARROW_LEFT:
                    if (e.isDown() || e.isHeld())
                    {
                        _cursor.sub(CAM_WIDTH / 4, 0);
                    }
                    continue;
                case NG_KEY_ARROW_RIGHT:
                    if (e.isDown() || e.isHeld())
                    {
                        _cursor.add(CAM_WIDTH / 4, 0);
                    }
                    continue;
                case NG_KEY_ARROW_DOWN:
                    if (e.isDown() || e.isHeld())
                    {
                        _cursor.sub(0, CAM_HEIGHT / 4);
                    }
                    continue;
                case NG_KEY_ARROW_UP:
                    if (e.isDown() || e.isHeld())
                    {
                        _cursor.add(0, CAM_HEIGHT / 4);
                    }
                    continue;
                case NG_KEY_O:
                    if (e.isDown())
                    {
                        resetCamera();
                    }
                    continue;
                case NG_KEY_S:
                    if (e.isDown())
                    {
                        if (_isControl)
                        {
                            engine->_world->saveMapAs('Z999');
                        }
                        else
                        {
                            engine->_world->saveMap();
                        }
                    }
                    continue;
                case NG_KEY_P:
                    if (e.isDown())
                    {
                        engine->_state ^= StudioEngineState_DisplayBox2D;
                    }
                    continue;
                case NG_KEY_G:
                    if (e.isDown())
                    {
                        engine->_state ^= StudioEngineState_DisplayGrid;
                    }
                    continue;
                case NG_KEY_ESCAPE:
                    _inputState = e.isUp() ? SIS_NONE : SIS_ESCAPE;
                    continue;
                default:
                    continue;
            }
        }
    }
    
    updateCamera(engine);
}

void StudioInputManager::setLiveInputMode(bool isLiveMode)
{
    _isLiveMode = isLiveMode;
    
    if (_isLiveMode)
    {
        _liveInput.clear();
    }
}

bool StudioInputManager::isLiveMode()
{
    return _isLiveMode;
}

bool StudioInputManager::isTimeToProcessInput()
{
    return _isTimeToProcessInput;
}

void StudioInputManager::onInputProcessed()
{
    _liveInput.clear();
    _isTimeToProcessInput = false;
}

int StudioInputManager::getMenuState()
{
    return _inputState;
}

std::string& StudioInputManager::getLiveInputRef()
{
    return _liveInput;
}

std::string StudioInputManager::getLiveInput()
{
    return _liveInput;
}

void StudioInputManager::updateCamera(StudioEngine *engine)
{
    int w = CAM_WIDTH * _scrollValue;
    int h = CAM_HEIGHT * _scrollValue;
    
    if (_lastScrollValue != _scrollValue)
    {
        CURSOR_CONVERTER->setCamSize(CAM_WIDTH * _lastScrollValue, CAM_HEIGHT * _lastScrollValue);
        Vector2& rc = CURSOR_INPUT_MANAGER->getCursorPosition();
        Vector2 c = CURSOR_CONVERTER->convert(rc);
        
        int dw = CAM_WIDTH * _lastScrollValue;
        int dh = CAM_HEIGHT * _lastScrollValue;
        
        float xFactor = c.getX() / dw;
        float yFactor = c.getY() / dh;
        
        Vector2 center = _cursor;
        center.add(dw * xFactor, dh * yFactor);
        center.sub(w * xFactor, h * yFactor);
        
        _cursor = center;
        
        _lastScrollValue = _scrollValue;
        CURSOR_CONVERTER->setCamSize(CAM_WIDTH * _scrollValue, CAM_HEIGHT * _scrollValue);
    }
    
    engine->_renderer->updateCamera(_cursor.getX(), _cursor.getY(), w, h);
}

void StudioInputManager::resetCamera()
{
    _scrollValue = 4;
    _lastScrollValue = 4;
    int w = CAM_WIDTH * _scrollValue;
    int h = CAM_HEIGHT * _scrollValue;
    CURSOR_INPUT_MANAGER->resetScrollValue();
    CURSOR_CONVERTER->setCamSize(w, h);
    _cursor.set(0, 0);
}

StudioInputManager::StudioInputManager() :
_downCursor(),
_dragCursor(),
_deltaCursor(),
_cursor(),
_upCursor(),
_liveInput(),
_inputState(SIS_NONE),
_isLiveMode(false),
_isTimeToProcessInput(false),
_isControl(false),
_scrollValue(1),
_lastScrollValue(1)
{
    resetCamera();
}
