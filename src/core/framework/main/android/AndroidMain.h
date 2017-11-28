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
    ndk_helper::GLContext* _glContext;
    android_app* _app;
    Engine* _engine;
    
    double _lastTime;
    float _deltaTime;
    bool _hasInitializedResources;
    bool _hasFocus;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    AndroidMain();
    ~AndroidMain();
    AndroidMain(const AndroidMain&);
    AndroidMain& operator=(const AndroidMain&);
};
