//
//  AndroidMain.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/main/android/AndroidMain.h"

#include "framework/main/portable/EngineController.h"
#include "framework/main/portable/Engine.h"

#include "framework/main/portable/EngineState.h"
#include "framework/input/CursorInputManager.h"
#include "framework/input/KeyboardInputManager.h"
#include "framework/file/android/AndroidAssetDataHandler.h"
#include "framework/audio/android/AndroidAudioEngineHelper.h"
#include "framework/util/Constants.h"
#include "framework/input/KeyboardLookup.h"
#include "framework/util/MathUtil.h"

#include <chrono>

AndroidMain *AndroidMain::getInstance()
{
    static AndroidMain instance = AndroidMain();
    return &instance;
}

void AndroidMain::handleCmd(struct android_app* app, int32_t cmd)
{
    AndroidMain* engine = (AndroidMain*) app->userData;
    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            LOGI("NG APP_CMD_SAVE_STATE");
            break;
        case APP_CMD_INIT_WINDOW:
            LOGI("NG APP_CMD_INIT_WINDOW");
            if (app->window != NULL)
            {
                engine->initDisplay();
                engine->drawFrame();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("NG APP_CMD_TERM_WINDOW");
            engine->termDisplay();
            engine->_hasFocus = false;
            break;
        case APP_CMD_DESTROY:
            LOGI("NG APP_CMD_DESTROY");
            break;
        case APP_CMD_STOP:
            LOGI("NG APP_CMD_STOP");
            break;
        case APP_CMD_GAINED_FOCUS:
            LOGI("NG APP_CMD_GAINED_FOCUS");
            engine->resume();
            engine->_hasFocus = true;
            break;
        case APP_CMD_LOST_FOCUS:
            LOGI("NG APP_CMD_LOST_FOCUS");
            engine->pause();
            engine->_hasFocus = false;
            engine->drawFrame();
            break;
        case APP_CMD_LOW_MEMORY:
            LOGI("NG APP_CMD_LOW_MEMORY");
            engine->trimMemory();
            break;
        case APP_CMD_RESUME:
            LOGI("NG APP_CMD_RESUME");
            engine->resume();
            break;
        case APP_CMD_PAUSE:
            LOGI("NG APP_CMD_PAUSE");
            engine->pause();
            break;
        case APP_CMD_INPUT_CHANGED:
            LOGI("NG APP_CMD_INPUT_CHANGED");
            break;
        case APP_CMD_WINDOW_RESIZED:
            LOGI("NG APP_CMD_WINDOW_RESIZED");
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            LOGI("NG APP_CMD_WINDOW_REDRAW_NEEDED");
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            LOGI("NG APP_CMD_CONTENT_RECT_CHANGED");
            break;
        case APP_CMD_CONFIG_CHANGED:
            LOGI("NG APP_CMD_CONFIG_CHANGED");
            break;
        case APP_CMD_START:
            LOGI("NG APP_CMD_START");
            break;
        default:
            LOGI("Default");
            break;
    }
}

int32_t AndroidMain::handleInput(android_app* app, AInputEvent* event)
{
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
        int32_t action = AMotionEvent_getAction(event);
        size_t pointerIndex = (size_t) ((action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
        uint32_t flags = (uint32_t) (action & AMOTION_EVENT_ACTION_MASK);
        
        size_t pointerCount = AMotionEvent_getPointerCount(event);
        
        switch (flags)
        {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
            {
                float x = AMotionEvent_getX(event, pointerIndex);
                float y = AMotionEvent_getY(event, pointerIndex);
                CURSOR_INPUT_MANAGER->onInput(CursorEventType_DOWN, x, y);
            }
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
            case AMOTION_EVENT_ACTION_CANCEL:
            {
                float x = AMotionEvent_getX(event, pointerIndex);
                float y = AMotionEvent_getY(event, pointerIndex);
                CURSOR_INPUT_MANAGER->onInput(CursorEventType_UP, x, y);
            }
                break;
            case AMOTION_EVENT_ACTION_MOVE:
            {
                for (size_t i = 0; i < pointerCount; ++i)
                {
                    pointerIndex = i;
                    float x = AMotionEvent_getX(event, pointerIndex);
                    float y = AMotionEvent_getY(event, pointerIndex);
                    CURSOR_INPUT_MANAGER->onInput(CursorEventType_DRAGGED, x, y);
                }
            }
                break;
            default:
                break;
        }
        
        return 1;
    }
    else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
    {
        if (AKeyEvent_getKeyCode(event) == AKEYCODE_BACK)
        {
            KEYBOARD_INPUT_MANAGER->onInput(NG_KEY_ESCAPE, true);
            
            return 1; // <-- prevent default handler
        }
    }
    
    return 0;
}

double timeNow()
{
    // Retrieves current time with a monotonic clock to ensure
    // time goes forward and is not subject to system changes.
    timespec lTimeVal;
    clock_gettime(CLOCK_MONOTONIC, &lTimeVal);
    return lTimeVal.tv_sec + (lTimeVal.tv_nsec * 1.0e-9);
}

void AndroidMain::exec(android_app* state, EngineController* engineController)
{
    if (!engineController)
    {
        return;
    }
    
#ifdef USE_NDK_PROFILER
    monstartup("native-lib.so");
#endif
    
    _app = state;
    
    JNIEnv *jni;
    _app->activity->vm->AttachCurrentThread(&jni, NULL);
    AndroidAssetDataHandler::getInstance()->init(jni, _app->activity->clazz);
    _app->activity->vm->DetachCurrentThread();
    
    _engine = new Engine(engineController);
    
    state->userData = this;
    state->onAppCmd = AndroidMain::handleCmd;
    state->onInputEvent = AndroidMain::handleInput;
    
    bool running = true;
    while (running)
    {
        // Checks elapsed time since last frame. It is important to
        // work on double with current time to avoid losing accuracy
        // Then we can go back to float for elapsed time.
        _deltaTime = timeNow() - _lastTime;
        _lastTime = timeNow();
        
        // Read all pending events.
        int events;
        android_poll_source* source;
        
        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ALooper_pollAll(isReady() ? 0 : -1, NULL, &events, (void**) &source)) >= 0)
        {
            if (source != NULL)
            {
                source->process(state, source);
            }
            
            if (state->destroyRequested != 0)
            {
                termDisplay();
                running = false;
                break;
            }
        }
        
        if (isReady())
        {
            drawFrame();
        }
    }
    
    _engine->releaseDeviceDependentResources();
    
    delete _engine;
    
    AndroidAssetDataHandler::getInstance()->deinit();
}

int AndroidMain::initDisplay()
{
    if (!_hasInitializedResources)
    {
        _glContext->Init(_app->window);
        
        loadResources();
        _hasInitializedResources = true;
    }
    else
    {
        // initialize OpenGL ES and EGL
        if (EGL_SUCCESS != _glContext->Resume(_app->window))
        {
            unloadResources();
            loadResources();
        }
        
        resume();
    }
    
    return 0;
}

void AndroidMain::loadResources()
{
    int width = _glContext->GetScreenWidth();
    int height = _glContext->GetScreenHeight();
    
    _engine->createDeviceDependentResources();
    _engine->createWindowSizeDependentResources(width, height);
}

void AndroidMain::unloadResources()
{
    _engine->releaseDeviceDependentResources();
}

void AndroidMain::drawFrame()
{
    using namespace std;
    using namespace std::chrono;
    
    int requestedAction = _engine->getRequestedAction();
    
    switch (requestedAction)
    {
        case REQUESTED_ACTION_EXIT:
            // TODO
            break;
        case REQUESTED_ACTION_UPDATE:
            break;
        default:
            _engine->clearRequestedAction();
            break;
    }
    
    _engine->update(_deltaTime);
    _engine->render();
    
    if (EGL_SUCCESS != _glContext->Swap())
    {
        unloadResources();
        loadResources();
    }
}

void AndroidMain::termDisplay()
{
    pause();
    
    _glContext->Suspend();
}

void AndroidMain::trimMemory()
{
    _glContext->Invalidate();
}

bool AndroidMain::isReady()
{
    return _hasFocus;
}

void AndroidMain::resume()
{
    if (!_hasInitializedResources)
    {
        return;
    }
    
    _engine->onResume();
    
    _hasFocus = true;
}

void AndroidMain::pause()
{
    if (!_hasInitializedResources)
    {
        return;
    }
    
    _engine->onPause();
    
    _hasFocus = false;
}

AndroidMain::AndroidMain() :
_glContext(ndk_helper::GLContext::GetInstance()),
_app(nullptr),
_engine(nullptr),
_lastTime(timeNow()),
_deltaTime(FRAME_RATE),
_hasInitializedResources(false),
_hasFocus(false)
{
    // Empty
}

AndroidMain::~AndroidMain()
{
    // Empty
}
