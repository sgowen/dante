//
//  main.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/18/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include <framework/main/android/AndroidMain.h>

#include <game/config/MainEngineController.h>

/**
 * This is the main entry point of a native application that is using
 * android_native_m_appglue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(android_app* state)
{
    app_dummy();

    MainEngineController controller;
    AndroidMain::getInstance()->exec(state, &controller);
}
