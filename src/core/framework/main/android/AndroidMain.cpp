//
//  AndroidMain.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/main/android/AndroidMain.h"

#include "framework/main/portable/Engine.h"

#include "framework/input/CursorInputManager.h"
#include "framework/input/KeyboardInputManager.h"
#include "framework/audio/superpowered/android/AndroidAudioEngineHelper.h"
#include "framework/file/android/AndroidAssetDataHandler.h"
#include "framework/graphics/opengl/OpenGLManager.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/input/KeyboardLookup.h"
#include "framework/math/MathUtil.h"

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
            engine->m_hasFocus = false;
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
            engine->m_hasFocus = true;
            break;
        case APP_CMD_LOST_FOCUS:
            LOGI("NG APP_CMD_LOST_FOCUS");
            engine->pause();
            engine->m_hasFocus = false;
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
                CURSOR_INPUT_MANAGER->onTouch(CursorEventType_DOWN, x, y);
            }
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
            case AMOTION_EVENT_ACTION_CANCEL:
            {
                float x = AMotionEvent_getX(event, pointerIndex);
                float y = AMotionEvent_getY(event, pointerIndex);
                CURSOR_INPUT_MANAGER->onTouch(CursorEventType_UP, x, y);
            }
                break;
            case AMOTION_EVENT_ACTION_MOVE:
            {
                for (size_t i = 0; i < pointerCount; ++i)
                {
                    pointerIndex = i;
                    float x = AMotionEvent_getX(event, pointerIndex);
                    float y = AMotionEvent_getY(event, pointerIndex);
                    CURSOR_INPUT_MANAGER->onTouch(CursorEventType_DRAGGED, x, y);
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

void AndroidMain::exec(android_app* state, Engine* engine)
{
    if (!engine)
    {
        return;
    }
    
    m_app = state;
    _engine = engine;
    
    state->userData = this;
    state->onAppCmd = AndroidMain::handleCmd;
    state->onInputEvent = AndroidMain::handleInput;
    
#ifdef USE_NDK_PROFILER
    monstartup("native-lib.so");
#endif
    
    while (1)
    {
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
                return;
            }
        }
        
        if (isReady())
        {
            drawFrame();
        }
    }
}

int AndroidMain::initDisplay()
{
    if (!m_hasInitializedResources)
    {
        m_glContext->Init(m_app->window);
        
        loadResources();
        m_hasInitializedResources = true;
    }
    else
    {
        // initialize OpenGL ES and EGL
        if (EGL_SUCCESS != m_glContext->Resume(m_app->window))
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
    JNIEnv *jni;
    m_app->activity->vm->AttachCurrentThread(&jni, NULL);
    
    ANDROID_AUDIO_ENGINE_HELPER->init(jni, m_app->activity->clazz);
    
    AndroidAssetDataHandler::getInstance()->init(jni, m_app->activity->clazz);
    
    int width = m_glContext->GetScreenWidth();
    int height = m_glContext->GetScreenHeight();
    
    _engine->createDeviceDependentResources();
    
    OGLManager->setScreenSize(width, height);
    
    if (m_glContext->GetScreenWidth() < 2560)
    {
        _engine->createWindowSizeDependentResources(width > 1280 ? 1280 : width, height > 720 ? 720 : height, width, height);
    }
    else
    {
        _engine->createWindowSizeDependentResources(width > 1440 ? 1440 : width, height > 900 ? 900 : height, width, height);
    }
    
    m_app->activity->vm->DetachCurrentThread();
    return;
}

void AndroidMain::unloadResources()
{
    ANDROID_AUDIO_ENGINE_HELPER->deinit();
    
    AndroidAssetDataHandler::getInstance()->deinit();
    
    _engine->releaseDeviceDependentResources();
}

std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

inline float approxRollingAverage(float avg, float newSample)
{
    static float numSamples = 60;
    
    avg -= avg / numSamples;
    avg += newSample / numSamples;
    
    return avg;
}

void AndroidMain::drawFrame()
{
    float deltaTime;
    
    using namespace std;
    using namespace std::chrono;
    
    t2 = steady_clock::now();
    duration<float> timeSpan = duration_cast<duration<float>>(t2 - t1);
    t1 = steady_clock::now();
    
    deltaTime = timeSpan.count();
    
    m_fAveragedDeltaTime = approxRollingAverage(m_fAveragedDeltaTime, deltaTime);
    
    deltaTime = clamp(m_fAveragedDeltaTime, 0.016f, 0.033f);
    
    if (m_fAveragedDeltaTime > 0.016f && m_fAveragedDeltaTime < 0.018f)
    {
        deltaTime = 0.016666666666667f;
    }
    
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
    
    _engine->update(deltaTime);
    
    _engine->render();
    
    if (EGL_SUCCESS != m_glContext->Swap())
    {
        unloadResources();
        loadResources();
    }
}

void AndroidMain::termDisplay()
{
    pause();
    
    m_glContext->Suspend();
}

void AndroidMain::trimMemory()
{
    m_glContext->Invalidate();
}

bool AndroidMain::isReady()
{
    return m_hasFocus;
}

void AndroidMain::resume()
{
    _engine->onResume();
    
    m_hasFocus = true;
}

void AndroidMain::pause()
{
    _engine->onPause();
    
    m_hasFocus = false;
}

AndroidMain::AndroidMain() :
m_glContext(ndk_helper::GLContext::GetInstance()),
m_app(nullptr),
_engine(nullptr),
m_fAveragedDeltaTime(0.016666666666667f),
m_hasInitializedResources(false),
m_hasFocus(false)
{
    // Empty
}

AndroidMain::~AndroidMain()
{
}
