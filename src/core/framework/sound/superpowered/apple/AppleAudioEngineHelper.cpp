//
//  AppleAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "AppleAudioEngineHelper.h"

#include "ISoundWrapper.h"

#include "SuperpoweredSoundWrapper.h"

#include "apple_audio_engine_helper.h"

AppleAudioEngineHelper* AppleAudioEngineHelper::getInstance()
{
    static AppleAudioEngineHelper instance = AppleAudioEngineHelper();
    return &instance;
}

void AppleAudioEngineHelper::update(int flags)
{
    // Empty
}

void AppleAudioEngineHelper::pause()
{
    // Empty
}

void AppleAudioEngineHelper::resume()
{
    // Empty
}

ISoundWrapper* AppleAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    const char* bundlePath = getBundlePathForSoundWithName(path);
    
    SuperpoweredSoundWrapper* sound = new SuperpoweredSoundWrapper(_superpoweredSoundManager, soundId, bundlePath, _sampleRate, numInstances);
    
    return sound;
}

ISoundWrapper* AppleAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

AppleAudioEngineHelper::AppleAudioEngineHelper() : IAudioEngineHelper(),
_superpoweredSoundManager(new SuperpoweredSoundManager(44100)),
_sampleRate(44100)
{
    initializeAudioSystemsWithSuperpoweredSoundManager(_superpoweredSoundManager, _sampleRate);
}

AppleAudioEngineHelper::~AppleAudioEngineHelper()
{
    deinitializeAudioSystems();
    
    delete _superpoweredSoundManager;
}
