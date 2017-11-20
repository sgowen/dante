//
//  GlfwMain.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include <GLFW/glfw3.h>

class Engine;

class GlfwMain
{
public:
    static GlfwMain* getInstance();
    
    static void error_callback(int error, const char* description);
    static void joystick_callback(int jid, int event);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    int exec(Engine* engine);
    
private:
    static int joysticks[GLFW_JOYSTICK_LAST + 1];
    static int joystick_count;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    GlfwMain();
    ~GlfwMain();
    GlfwMain(const GlfwMain&);
    GlfwMain& operator=(const GlfwMain&);
};
