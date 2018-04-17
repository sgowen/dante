//
//  PlatformHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 12/10/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__PlatformHelper__
#define __noctisgames__PlatformHelper__

#define NG_PLATFORM_IOS 1
#define NG_PLATFORM_MAC_OS 2
#define NG_PLATFORM_ANDROID 3
#define NG_PLATFORM_LINUX 4
#define NG_PLATFORM_WINDOWS 5

#if defined __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #define NG_PLATFORM NG_PLATFORM_IOS
    #elif TARGET_OS_OSX
        #define NG_PLATFORM NG_PLATFORM_MAC_OS
    #endif
    #elif defined __ANDROID__
        #define NG_PLATFORM NG_PLATFORM_ANDROID
    #elif defined __linux__
        #define NG_PLATFORM NG_PLATFORM_LINUX
    #elif defined _WIN32
        #define NG_PLATFORM NG_PLATFORM_WINDOWS
#endif

class PlatformHelper
{
public:
    static int getPlatform()
    {
        return NG_PLATFORM;
    }
};

#endif /* defined(__noctisgames__PlatformHelper__) */
