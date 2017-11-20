//
//  AndroidMain.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "android_native_app_glue.h"
#include "GLContext.h"

class Engine;

class AndroidMain
{
public:
    static AndroidMain* getInstance();
    
    static void handleCmd(struct android_app* app, int32_t cmd);
    static int32_t handleInput(android_app* app, AInputEvent* event);
    
    void exec(android_app* state, Engine* engine);
    
    int initDisplay();
    void loadResources();
    void unloadResources();
    void drawFrame();
    void termDisplay();
    void trimMemory();
    bool isReady();
    
    void resume();
    void pause();
    
private:
    ndk_helper::GLContext* m_glContext;
    android_app* m_app;
    Engine* _engine;
    
    float m_fAveragedDeltaTime;
    bool m_hasInitializedResources;
    bool m_hasFocus;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    AndroidMain();
    ~AndroidMain();
    AndroidMain(const AndroidMain&);
    AndroidMain& operator=(const AndroidMain&);
};
