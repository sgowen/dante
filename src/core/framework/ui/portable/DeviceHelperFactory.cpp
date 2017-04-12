//
//  DeviceHelperFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "DeviceHelperFactory.h"

DeviceHelperFactory* DeviceHelperFactory::getInstance()
{
    static DeviceHelperFactory instance = DeviceHelperFactory();
    
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__
#include "OpenGLDeviceHelper.h"
IDeviceHelper* DeviceHelperFactory::createDeviceHelper() { return new OpenGLDeviceHelper(); }
#elif defined _WIN32
#include "Direct3DDeviceHelper.h"
IDeviceHelper* DeviceHelperFactory::createDeviceHelper() { return new Direct3DDeviceHelper(); }
#endif

DeviceHelperFactory::DeviceHelperFactory()
{
    // Hide Constructor for Singleton
}
