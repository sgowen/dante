//
//  InstanceManager.cpp
//  dante
//
//  Created by Stephen Gowen on 6/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "InstanceManager.h"

#include "World.h"

World* InstanceManager::getClientWorld()
{
    static World instance = World();
    return &instance;
}

World* InstanceManager::getServerWorld()
{
    static World instance = World();
    return &instance;
}

InstanceManager::InstanceManager()
{
    // Empty
}

InstanceManager::~InstanceManager()
{
    // Empty
}
