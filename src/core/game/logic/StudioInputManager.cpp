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

void StudioInputManager::setEngine(StudioEngine* inValue)
{
    _engine = inValue;
}

void StudioInputManager::update()
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
                        char key = charForKey(keyboardEvent->getKey());
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
        _rawScrollValue = clamp(_rawScrollValue + CURSOR_INPUT_MANAGER->getScrollWheelValue(), 16, 1);
        CURSOR_INPUT_MANAGER->resetScrollValue();
        _scrollValue = clamp(_rawScrollValue, 16, 1);
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
                    _isControl = e.isPressed();
                    continue;
                case NG_KEY_ZERO:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer0 : 0;
                    continue;
                case NG_KEY_ONE:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer1 : 0;
                    continue;
                case NG_KEY_TWO:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer2 : 0;
                    continue;
                case NG_KEY_THREE:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer3 : 0;
                    continue;
                case NG_KEY_FOUR:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer4 : 0;
                    continue;
                case NG_KEY_FIVE:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer5 : 0;
                    continue;
                case NG_KEY_SIX:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer6 : 0;
                    continue;
                case NG_KEY_SEVEN:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer7 : 0;
                    continue;
                case NG_KEY_EIGHT:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer8 : 0;
                    continue;
                case NG_KEY_NINE:
                    _engine->_state ^= e.isDown() ? StudioEngineState_Layer9 : 0;
                    continue;
                case NG_KEY_ARROW_LEFT:
                    isPanningLeft = e.isPressed();
                    continue;
                case NG_KEY_ARROW_RIGHT:
                    isPanningRight = e.isPressed();
                    continue;
                case NG_KEY_ARROW_DOWN:
                    isPanningDown = e.isPressed();
                    continue;
                case NG_KEY_ARROW_UP:
                    isPanningUp = e.isPressed();
                    continue;
                case NG_KEY_C:
                    _engine->_state ^= e.isDown() ? StudioEngineState_DisplayControls : 0;
                    continue;
                case NG_KEY_A:
                    _engine->_state ^= e.isDown() ? StudioEngineState_DisplayAssets : 0;
                    continue;
                case NG_KEY_E:
                    _engine->_state ^= e.isDown() ? StudioEngineState_DisplayEntities : 0;
                    continue;
                case NG_KEY_R:
                    if (e.isDown())
                    {
                        resetCamera();
                    }
                    continue;
                case NG_KEY_N:
                    if (e.isDown())
                    {
                        /// TODO, prompt for new key / filename combo, update maps.cfg, and then load it
                    }
                    continue;
                case NG_KEY_L:
                    if (e.isDown())
                    {
                        /// TODO, prompt user to pick a map to load
                    }
                    continue;
                case NG_KEY_S:
                    if (e.isDown())
                    {
                        if (_isControl)
                        {
                            /// TODO, prompt for new key / filename combo and then save
                        }
                        else
                        {
                            _engine->_world->saveMap();
                            _engine->_renderer->displayToast(StringUtil::format("%s saved!", _engine->_world->getMapName().c_str()).c_str());
                        }
                    }
                    continue;
                case NG_KEY_P:
                    _engine->_state ^= e.isDown() ? StudioEngineState_DisplayParallax : 0;
                    continue;
                case NG_KEY_B:
                    _engine->_state ^= e.isDown() ? StudioEngineState_DisplayBox2D : 0;
                    continue;
                case NG_KEY_G:
                    _engine->_state ^= e.isDown() ? StudioEngineState_DisplayGrid : 0;
                    continue;
                case NG_KEY_ESCAPE:
                    _inputState = e.isDown() ? SIS_ESCAPE : SIS_NONE;
                    continue;
                default:
                    continue;
            }
        }
    }
    
    updateCamera();
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

void StudioInputManager::updateCamera()
{
    int w = CAM_WIDTH * _scrollValue;
    int h = CAM_HEIGHT * _scrollValue;
    float topPan = h * 0.9f;
    float bottomPan = h * 0.1f;
    float rightPan = w * 0.9f;
    float leftPan = w * 0.1f;
    
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
        
        _rawScrollValue = _scrollValue;
        _lastScrollValue = _scrollValue;
        CURSOR_CONVERTER->setCamSize(CAM_WIDTH * _scrollValue, CAM_HEIGHT * _scrollValue);
    }
    
    {
        /// Update Camera based on mouse being near edges
        Vector2& rc = CURSOR_INPUT_MANAGER->getCursorPosition();
        Vector2 c = CURSOR_CONVERTER->convert(rc);
        if (c.getY() > topPan || isPanningUp)
        {
            _cursor.add(0, h / CAM_HEIGHT / 2.0f);
        }
        if (c.getY() < bottomPan || isPanningDown)
        {
            _cursor.sub(0, h / CAM_HEIGHT / 2.0f);
        }
        if (c.getX() > rightPan || isPanningRight)
        {
            _cursor.add(w / CAM_WIDTH / 2.0f, 0);
        }
        if (c.getX() < leftPan || isPanningLeft)
        {
            _cursor.sub(w / CAM_WIDTH / 2.0f, 0);
        }
    }
    
    _engine->_renderer->update(_cursor.getX(), _cursor.getY(), w, h, _scrollValue);
}

void StudioInputManager::resetCamera()
{
    _rawScrollValue = 1;
    _scrollValue = 1;
    _lastScrollValue = 1;
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
_rawScrollValue(1),
_scrollValue(1),
_lastScrollValue(1),
isPanningUp(false),
isPanningDown(false),
isPanningRight(false),
isPanningLeft(false),
_engine(NULL)
{
    resetCamera();
}
