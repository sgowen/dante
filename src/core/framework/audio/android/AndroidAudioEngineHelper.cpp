//
//  AndroidAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/android/AndroidAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"
#include "framework/audio/android/SoundService.hpp"

#include "framework/audio/android/AndroidSoundWrapper.h"
#include "framework/util/NGSTDUtil.h"

#include <assert.h>
#include <sstream>

AndroidAudioEngineHelper* AndroidAudioEngineHelper::getInstance()
{
    static AndroidAudioEngineHelper instance = AndroidAudioEngineHelper();
    return &instance;
}

void AndroidAudioEngineHelper::update(int flags)
{
    // Empty
}

void AndroidAudioEngineHelper::pause()
{
    // Empty
}

void AndroidAudioEngineHelper::resume()
{
    // Empty
}

SoundWrapper* AndroidAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    AndroidSoundWrapper* sound = new AndroidSoundWrapper(_soundService, soundId, path, numInstances);
    
    return sound;
}

SoundWrapper* AndroidAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

AndroidAudioEngineHelper::AndroidAudioEngineHelper() : AudioEngineHelper(), _soundService(new SoundService())
{
    _soundService->start();
}

AndroidAudioEngineHelper::~AndroidAudioEngineHelper()
{
    _soundService->stop();
    
    delete _soundService;
}
