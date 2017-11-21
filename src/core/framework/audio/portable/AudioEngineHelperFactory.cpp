//
//  AudioEngineHelperFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/portable/AudioEngineHelperFactory.h"

#if defined __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #import "AppleAudioEngineHelper.h"
    #elif TARGET_OS_OSX
        #import "MacAudioEngineHelper.h"
    #endif
#elif defined __ANDROID__
    #include "framework/audio/superpowered/android/AndroidAudioEngineHelper.h"
#elif defined _WIN32
    #include "framework/audio/directx/DirectXAudioEngineHelper.h"
#else
    #include "framework/audio/portable/NullAudioEngineHelper.h"
#endif

#include <assert.h>

AudioEngineHelperFactory* AudioEngineHelperFactory::getInstance()
{
    static AudioEngineHelperFactory instance = AudioEngineHelperFactory();
    return &instance;
}

AudioEngineHelper* AudioEngineHelperFactory::createAudioEngineHelper()
{
#if defined __APPLE__
    #if TARGET_OS_IPHONE
        return AppleAudioEngineHelper::getInstance();
    #elif TARGET_OS_OSX
        return MacAudioEngineHelper::getInstance();
    #endif
#elif defined __ANDROID__
    return AndroidAudioEngineHelper::getInstance();
#elif defined __linux__
    return NullAudioEngineHelper::getInstance();
#elif defined _WIN32
	return DirectXAudioEngineHelper::getInstance();
#else
    return NullAudioEngineHelper::getInstance();
#endif

    assert(false);
}

AudioEngineHelperFactory::AudioEngineHelperFactory()
{
    // Hide Constructor for Singleton
}
