//
//  GlfwMain.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/portable/NGGraphics.h"

#include <GLFW/glfw3.h>

class EngineController;

class GlfwMain
{
public:
    static GlfwMain* getInstance();

    static void error_callback(int error, const char* description);
    static void joystick_callback(int jid, int event);
    static void mouse_scroll_callback(GLFWwindow* window, double x, double y);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_cursor_pos_callback(GLFWwindow*, double x, double y);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    int exec(EngineController* engineController);

private:
    static int joysticks[GLFW_JOYSTICK_LAST + 1];
    static int joystick_count;

    // ctor, copy ctor, and assignment should be private in a Singleton
    GlfwMain();
    ~GlfwMain();
    GlfwMain(const GlfwMain&);
    GlfwMain& operator=(const GlfwMain&);
};
