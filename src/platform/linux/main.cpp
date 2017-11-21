//
//  main.cpp
//  dante
//
//  Created by Stephen Gowen on 11/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/main/glfw/GlfwMain.h"

#include "game/logic/MainEngine.h"

int main(void)
{
    return GlfwMain::getInstance()->exec(new MainEngine());
}
