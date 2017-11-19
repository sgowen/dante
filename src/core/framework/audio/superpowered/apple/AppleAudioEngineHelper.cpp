//
//  AppleAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/superpowered/apple/AppleAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"

#include "framework/audio/superpowered/portable/SuperpoweredSoundWrapper.h"

#include "framework/audio/superpowered/apple/apple_audio_engine_helper.h"

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

SoundWrapper* AppleAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    const char* bundlePath = getBundlePathForSoundWithName(path);
    
    SuperpoweredSoundWrapper* sound = new SuperpoweredSoundWrapper(_superpoweredSoundManager, soundId, bundlePath, _sampleRate, numInstances);
    
    return sound;
}

SoundWrapper* AppleAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

AppleAudioEngineHelper::AppleAudioEngineHelper() : AudioEngineHelper(),
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
