//
//  AudioEngineHelperFactory.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/audio/portable/AudioEngineHelperFactory.h>

#if defined __APPLE__
    #include <framework/audio/apple/AppleAudioEngineHelper.h>
#elif defined __ANDROID__
    #include <framework/audio/android/AndroidAudioEngineHelper.h>
#elif defined __linux__
    #include <framework/audio/linux/LinuxAudioEngineHelper.h>
#elif defined _WIN32
    #include <framework/audio/directx/DirectXAudioEngineHelper.h>
#else
    #include <framework/audio/portable/NullAudioEngineHelper.h>
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
    return new AppleAudioEngineHelper();
#elif defined __ANDROID__
    return new AndroidAudioEngineHelper();
#elif defined __linux__
    return new LinuxAudioEngineHelper();
#elif defined _WIN32
	return new DirectXAudioEngineHelper();
#else
    return new NullAudioEngineHelper();
#endif

    assert(false);
}

AudioEngineHelperFactory::AudioEngineHelperFactory()
{
    // Hide Constructor for Singleton
}
