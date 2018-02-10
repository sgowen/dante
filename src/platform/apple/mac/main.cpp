//
//  main.cpp
//  dante
//
//  Created by Stephen Gowen on 11/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include <framework/main/glfw/GlfwMain.h>

#include <game/logic/MainEngineController.h>

int main(void)
{
    MainEngineController controller;
    return GlfwMain::getInstance()->exec(&controller);
}
