//
//  AudioEngineHelperFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "AudioEngineHelperFactory.h"

#if defined __APPLE__
#include "AppleAudioEngineHelper.h"
#elif defined __ANDROID__
#include "AndroidAudioEngineHelper.h"
#elif defined _WIN32
#include "WinAudioEngineHelper.h"
#endif

#include <assert.h>

AudioEngineHelperFactory* AudioEngineHelperFactory::getInstance()
{
    static AudioEngineHelperFactory instance = AudioEngineHelperFactory();
    return &instance;
}

IAudioEngineHelper* AudioEngineHelperFactory::createAudioEngineHelper()
{
#if defined __APPLE__
    return AppleAudioEngineHelper::getInstance();
#elif defined __ANDROID__
    return AndroidAudioEngineHelper::getInstance();
#elif defined _WIN32
    return WinAudioEngineHelper::getInstance();
#endif
    
    assert(false);
}

AudioEngineHelperFactory::AudioEngineHelperFactory()
{
    // Hide Constructor for Singleton
}
